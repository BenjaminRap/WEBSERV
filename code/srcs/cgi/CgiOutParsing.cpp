#include "CgiOut.hpp"
#include "FlowBuffer.hpp"
#include "Status.hpp"
#include "requestStatusCode.hpp"
#include <cstdlib>

unsigned long	stringToULongBase(const std::string& str, int (&isInBase)(int character), int base);
void			setFirstPart(std::string& result, const Status& status, const std::string& autoIndexPage, const Headers& headers, bool hasBody);
void			addDefaultHeaders(Headers& headers, const Status* status);
FileFd*			getErrorPage(const Status** currentStatus, const ServerConfiguration& serverConfiguration);
std::string		sizeTToString(size_t value);

uint16_t	CgiOut::checkHeaders(void)
{
	if (_headers.getHeader("content-type") == NULL)
		return (HTTP_BAD_GATEWAY);
	const std::string*	contentLength = _headers.getHeader("content-length");
	const std::string*	transferEncoding = _headers.getHeader("transfer-encoding");
	if (contentLength != NULL && transferEncoding != NULL)
		return (HTTP_BAD_GATEWAY);
	if (contentLength != NULL)
	{
		const unsigned long	size = stringToULongBase(*contentLength, std::isdigit, 10);
		if (size == (unsigned long)-1)
			return (HTTP_BAD_GATEWAY);
	}
	else if (transferEncoding == NULL)
	{
		_srcFile = FileFd::getTemporaryFile(_tempName, O_WRONLY);
		if (_srcFile == NULL)
			return (HTTP_INTERNAL_SERVER_ERROR);
		_state = CGI_TO_TEMP;
	}
	return (HTTP_OK);
}

uint16_t	CgiOut::getStatusCode(void)
{
	const std::string*	status = _headers.getHeader("status");
	if (status != NULL)
	{
		char	*end;
		const unsigned long	code = std::strtoul(status->c_str(), &end, 10);

		_headers.erase("status");
		if (end != status->c_str() + 2 || code < 100 || code >= 600)
		{
			_error = true;
			return (HTTP_BAD_GATEWAY);
		}
		return (code);
	}
	if (_headers.getHeader("Location") != NULL)
		return (HTTP_FOUND);
	return (HTTP_OK);
}

void	CgiOut::setErrorPage(const Status** currentStatus)
{
	_headers.clear();
	if (_srcFile != NULL)
		delete _srcFile;
	_srcFile = getErrorPage(currentStatus, _serverConf);
	addDefaultHeaders(_headers, *currentStatus);

	const size_t	size = (_srcFile)
		? _srcFile->getSize() :
		(*currentStatus)->getErrorPage().size();

	_headers["content-length"] = sizeTToString(size);
}

void	CgiOut::generateFirstPart(void)
{
	const Status*	status;

	status = Status::getStatus(_code);
	if (status == NULL)
	{
		status = Status::getStatus(HTTP_BAD_GATEWAY);
		_error = true;
	}
	if (_error)
		setErrorPage(&status);
	const bool	hasBody = (_srcFile != NULL || _error == false);

	setFirstPart(_firstPart, *status, "", _headers, hasBody);
	_state = WRITE_FIRST_PART;
}

void		CgiOut::readHeaders(void)
{
	char	*begin;
	char	*end;

	while (_flowBuf.getLine(&begin, &end))
	{
		if (end == begin + 1 && *begin == '\r')
		{
			_code = checkHeaders();
			if (_code != HTTP_OK)
			{
				_error = true;
				generateFirstPart();
				return ;
			}
			_code = getStatusCode();
			if (_state != CGI_TO_TEMP || _error)
				generateFirstPart();
			return ;
		}
		_code = _headers.parseHeader(begin, end);
		if (_code != HTTP_OK)
		{
			_error = true;
			generateFirstPart();
			return ;
		}
	}
}
