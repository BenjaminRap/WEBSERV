#include <stdint.h>               // for uint16_t
#include <cctype>                 // for isdigit
#include <cstdlib>                // for NULL, strtoul, size_t
#include <map>                    // for map
#include <string>                 // for basic_string, string

#include "CgiOut.hpp"             // for CgiOut, CgiOutState
#include "FileFd.hpp"             // for FileFd
#include "FlowBuffer.hpp"         // for FlowBuffer
#include "Headers.hpp"            // for Headers
#include "Status.hpp"             // for Status
#include "requestStatusCode.hpp"  // for HTTP_BAD_GATEWAY, HTTP_OK, HTTP_FOUND

class ServerConfiguration;  // lines 15-15

unsigned long	stringToULongBase(const std::string& str, int (&isInBase)(int character), int base);
std::string&	getFirstPart(const Status& status, const Headers& headers, const char* bodyBegin, const char* bodyEnd);
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
		_srcFile = FileFd::getTemporaryFile(_tempName);
		if (_srcFile == NULL)
		{
			_tempName[0] = '\0';
			return (HTTP_INTERNAL_SERVER_ERROR);
		}
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

		if (end != status->c_str() + 3 || code < 100 || code >= 600)
		{
			_error = true;
			return (HTTP_BAD_GATEWAY);
		}
		_headers.erase("status");
		return (code);
	}
	if (_headers.getHeader("location") != NULL)
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
		(*currentStatus)->getPage().size();

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
	char*	remainingBegin;
	char*	remainingEnd;
	const char*	bodyBegin;
	const char*	bodyEnd;

	_flowBuf.getContent(&remainingBegin, &remainingEnd);

	if (_srcFile != NULL)
	{
		bodyBegin = NULL;
		bodyEnd = NULL;
	}
	else if (_error && _srcFile == NULL)
	{
		const std::string&	errorPage = status->getPage();

		bodyBegin = errorPage.c_str();
		bodyEnd = bodyBegin + errorPage.size();
	}
	else
	{
		bodyBegin = remainingBegin;
		bodyEnd = remainingEnd;
	}
	_firstPart = getFirstPart(*status, _headers, bodyBegin, bodyEnd);
	_state = WRITE_FIRST_PART;
	_canWrite = true;
}

void		CgiOut::readHeaders(void)
{
	char	*begin;
	char	*end;

	_code = HTTP_OK;
	while (_flowBuf.getLine(&begin, &end))
	{
		if (end == begin + 1 && *begin == '\r')
		{
			_code = checkHeaders();
			if (_code != HTTP_OK)
				break ;
			_code = getStatusCode();
			if (_state != CGI_TO_TEMP || _error)
				generateFirstPart();
			return ;
		}
		if (_headers.parseHeader(begin, end) != HTTP_OK)
		{
			_code = HTTP_BAD_GATEWAY;
			break ;
		}
	}
	if (_flowBuf.isBufferFull())
		_code = HTTP_BAD_GATEWAY;
	if (_code != HTTP_OK)
	{
		_error = true;
		generateFirstPart();
	}
}
