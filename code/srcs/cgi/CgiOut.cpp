#include <cstdlib>					// for std::strtoul
#include <sys/epoll.h>				// for EPOLLIN

#include "CgiOut.hpp"				// for CgiOut
#include "FlowBuffer.hpp"			// for FlowBUffer
#include "Status.hpp"				// for Status
#include "requestStatusCode.hpp"	// for HTTP_...

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer&	responseFlowBuffer,
	const ServerConfiguration& serverConfiguration
) :
	AFdData(fd, ePollHandler, CGIOUT),
	_flowBuf(responseFlowBuffer),
	_firstPart(),
	_charsWritten(0),
	_headers(),
	_srcFile(NULL),
	_state(READ_HEADER),
	_code(0),
	_error(false),
	_serverConf(serverConfiguration)
{
	_tempName[0] = '\0';
}

CgiOut::~CgiOut()
{
	if (_srcFile != NULL)
	{
		std::remove(_tempName);
		delete _srcFile;
	}
}

unsigned long	stringToULongBase(const std::string& str, int (&isInBase)(int character), int base);

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
		_state = CGI_TO_BUFFER;
	}
	else if (transferEncoding == NULL || *transferEncoding != "chunked")
		_state = CGI_TO_BUFFER;
	else
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

void		addDefaultHeaders(Headers& headers, const Status* status);
FileFd*		getErrorPage(const Status** currentStatus, const ServerConfiguration& serverConfiguration);
std::string	sizeTToString(size_t value);

void	CgiOut::setErrorPage(const Status** currentStatus)
{
	_headers.clear();
	addDefaultHeaders(_headers, *currentStatus);
	if (_srcFile != NULL)
		delete _srcFile;
	_srcFile = getErrorPage(currentStatus, _serverConf);
	const size_t	size = (_srcFile)
		? _srcFile->getSize() :
		(*currentStatus)->getErrorPage().size();
	_headers["content-length"] = sizeTToString(size);
}

void	setFirstPart
(
	std::string& result,
	const Status& status,
	const std::string& autoIndexPage,
	const Headers& headers,
	bool hasBody
);

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

void	CgiOut::handleClosingCgi()
{
	if (_state == READ_HEADER)
	{
		_code = HTTP_BAD_GATEWAY;
		_error = true;
		generateFirstPart();
	}
	else if (_state == CGI_TO_TEMP)
	{
		delete _srcFile;
		_srcFile = FileFd::getTemporaryFile(_tempName, O_RDONLY);
		if (_srcFile == NULL)
		{
			_code = HTTP_INTERNAL_SERVER_ERROR;
			_error = true;
		}
		else
			_headers["content-length"] = sizeTToString(_srcFile->getSize());
		generateFirstPart();
	}
	else
	{
		_isActive = false;
		_state = DONE;
	}
}

void	CgiOut::readFromCgi()
{
	if (_error || _state == FILE_TO_BUFFER || !_isActive || _state == DONE)
		return ;
	const FlowState flowState = _flowBuf.srcToBuff(getFd());

	if (flowState == FLOW_BUFFER_FULL || flowState == FLOW_MORE)
		return ;
	_isActive = false;
	_state = DONE;
}

void	CgiOut::writeToTemp(void)
{
	if (_flowBuf.buffToDest(_srcFile->getFd()) == FLOW_DONE)
		return ;
	delete _srcFile;
	_srcFile = NULL;
	_code = HTTP_INTERNAL_SERVER_ERROR;
	_error = true;
	generateFirstPart();
}

void	CgiOut::writeToBuff(void)
{
	const int		fd = _srcFile ? _srcFile->getFd() : getFd();
	const FlowState	flowState = _flowBuf.srcToBuff(fd);

	if (flowState == FLOW_BUFFER_FULL || flowState == FLOW_MORE)
		return ;
	_isActive = false;
	_state = DONE;
}

void	CgiOut::callback(uint32_t events)
{
	if (!_isActive || _state == DONE)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
		handleClosingCgi();
	if (events & EPOLLIN)
		readFromCgi();
	if (_state == READ_HEADER)
		readHeaders();
	if (_state == CGI_TO_TEMP)
		writeToTemp();
	if (_state == FILE_TO_BUFFER || _state == CGI_TO_BUFFER)
		writeToBuff();
}
