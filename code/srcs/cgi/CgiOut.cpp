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
}

CgiOut::~CgiOut()
{
	if (_srcFile != NULL)
		fclose(_srcFile);
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
		_srcFile = std::tmpfile();
		if (_srcFile == NULL)
			return (HTTP_INTERNAL_SERVER_ERROR);
		_state = CGI_TO_FILE;
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
			return (-1);
		return (code);
	}
	if (_headers.getHeader("Location") != NULL)
		return (HTTP_FOUND);
	return (HTTP_OK);
}

void	setFirstPart
(
	std::string& result,
	const Status& status,
	const std::string& autoIndexPage,
	const Headers& headers,
	bool hasBody
);

uint16_t	getStatusCodeFromErrno(int errnoValue);

void	CgiOut::generateFirstPart(void)
{
	const Status*	status;

	status = Status::getStatus(_code);
	if (_error)
	{
		if (_srcFile != NULL)
			fclose(_srcFile);
		const std::string* errorPage = _serverConf.getErrorPage(_code);

		if (errorPage != NULL)
		{
			_srcFile = fopen(errorPage->c_str(), "r");
			if (_srcFile == NULL)
			{
				_code = getStatusCodeFromErrno(errno);
				status = Status::getStatus(_code);
			}
		}
	}
	if (status == NULL)
	{
		status = Status::getStatus(HTTP_BAD_GATEWAY);
		_error = true;
	}
	const bool	hasBody = (_srcFile != NULL || _error == false);

	setFirstPart(_firstPart, *status, "", _headers, hasBody);
}

ssize_t		CgiOut::readHeader(void)
{
	char	*begin;
	char	*end;

	if (!_flowBuf.getLine(&begin, &end))
		return (0);

	if (begin == end - 1 && *begin == '\r')
	{
		_code = checkHeaders();
		if (_code != HTTP_OK)
			return (-1);
		_code = getStatusCode();
		if (_state != CGI_TO_FILE || _code == (uint16_t)-1)
			generateFirstPart();
		return (1);
	}
	_code = _headers.parseHeader(begin, end);
	if (_code != HTTP_OK)
		return (-1);
	return (1);
}

void	CgiOut::callback(uint32_t events)
{
	if (!_isActive)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
	{
		if (_state == READ_HEADER)
			_code = HTTP_BAD_GATEWAY;
		else if (_state == CGI_TO_FILE)
			_state = FILE_TO_BUFFER;
		else
			_isActive = false;
		return ;
	}
	if (events & EPOLLIN)
	{
		const FlowState flowState = _flowBuf.srcToBuff<int>(getFd());

		if (flowState == FLOW_ERROR || flowState == FLOW_DONE)
		{
			_isActive = false;
			return ;
		}
	}
	if (_flowBuf.isBufferEmpty())
		return ;
	while (_state == READ_HEADER)
	{
		readHeader();
	}
}
