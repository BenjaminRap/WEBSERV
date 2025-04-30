#include <cstdlib>					// for std::strtoul
#include <stdexcept>				// for std::logic_error
#include <sys/epoll.h>				// for EPOLLIN

#include "CgiOut.hpp"				// for CgiOut
#include "FlowBuffer.hpp"			// for FlowBUffer
#include "Status.hpp"				// for Status
#include "requestStatusCode.hpp"	// for HTTP_...

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer&	responseFlowBuffer
) :
	AFdData(fd, ePollHandler),
	_flowBuf(responseFlowBuffer),
	_firstPart(),
	_charsWritten(0),
	_headers(),
	_tempFile(NULL),
	_state(READ_HEADER),
	_code(0)
{
}

CgiOut::~CgiOut()
{

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
		_tempFile = std::tmpfile();
		if (_tempFile == NULL)
			return (HTTP_INTERNAL_SERVER_ERROR);
		_state = CGI_TO_TEMP;
	}

	const std::string*	status = _headers.getHeader("status");
	if (status != NULL)
	{
		char	*end;
		const unsigned long	code = std::strtoul(status->c_str(), &end, 10);

		_headers.erase("status");
		if (end != status->c_str() + 2 || code < 100 || code >= 600)
			return (HTTP_BAD_GATEWAY);
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

void	CgiOut::generateFirstPart(void)
{
	try
	{
		const Status&	status = Status::getStatus(_code);
		setFirstPart(_firstPart, status, "", _headers, true);
	}
	catch (std::logic_error& err)
	{
		const Status&	status = Status::getStatus(HTTP_BAD_GATEWAY);
		setFirstPart(_firstPart, status, "", _headers, false);
	}
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

		if (_state != CGI_TO_TEMP)
			generateFirstPart();
		return (1);
	}
	const uint16_t	code = _headers.parseHeader(begin, end);
	if (code != HTTP_OK)
	{
		_code = code;
		return (-1);
	}
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
		else if (_state == CGI_TO_TEMP)
			_state = TEMP_TO_BUFFER;
		else
			_isActive = false;
		return ;
	}
	const FlowState flowState = _flowBuf.srcToBuff<int>(getFd());

	if (flowState == FLOW_ERROR || flowState == FLOW_DONE)
	{
		_isActive = false;
		return ;
	}
	while (_state == READ_HEADER)
	{
		readHeader();
	}
}
