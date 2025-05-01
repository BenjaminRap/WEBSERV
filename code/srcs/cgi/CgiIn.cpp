#include <sys/epoll.h>				// for EPOLLOUT

#include "CgiIn.hpp"				// for CgiIn
#include "ChunkedBody.hpp"			// for ChunkedBody
#include "FlowBuffer.hpp"			// for FlowBUffer
#include "ABody.hpp"				// for ABody
#include "ConnectedSocketData.hpp"	// for ConnectedSocketData
#include "Response.hpp"				// for Response
#include "requestStatusCode.hpp"	// for HTTP_...

CgiIn::CgiIn
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer& requestFlowBuffer,
	ABody& body,
	ConnectedSocketData& connectedSocketData,
	Response& currentResponse
) :
	AFdData(fd, ePollHandler, CGIIN),
	_flowBuf(requestFlowBuffer),
	_body(body),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse),
	_tempFile(NULL),
	_tempFileSize(-1),
	_state(BUF_TO_CGI)
{
	if (dynamic_cast<ChunkedBody*>(&_body) == NULL)
		return ;
	_tempFile = std::tmpfile();
	_state =  BUF_TO_TEMP;
	if (_tempFile == NULL)
		throw std::runtime_error("error creating a temporary file !");
}

CgiIn::~CgiIn()
{
	if (_tempFile != NULL)
		fclose(_tempFile);
}

ssize_t	fwriteChar(FILE* file, void* buffer, size_t length)
{
	const size_t	written = fwrite(buffer, 1, length, file);

	if (written < length)
		return (-1);
	return (written);
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body);


void	CgiIn::setFinished(uint16_t code)
{
	_response.setResponse(code);
	_connectedSocketData.readNextRequests(_response, REQUEST_DONE);
	_isActive = false;
}

void	CgiIn::redirectToTemp(void)
{
	const FlowState	flowState = _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);
	const uint16_t	code = getCodeIfFinished(true, flowState, _body);

	if (code == 0)
		return ;
	if (code == HTTP_OK)
	{
		_tempFileSize = std::ftell(_tempFile);
		std::rewind(_tempFile);
		_state = TEMP_TO_CGI;
	}
	else
		setFinished(code);
}

void	CgiIn::callback(uint32_t events)
{
	if (!_isActive)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
	{
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
		return ;
	}
	if (_state == BUF_TO_TEMP)
	{
		redirectToTemp();
		return ;
	}
	if (!(events & EPOLLIN))
		return ;

	const FlowState	flowState = (TEMP_TO_CGI)
		? _flowBuf.redirect(_tempFile, getFd(), write, fwriteChar)
		: _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);

	const uint16_t	code = getCodeIfFinished(true, flowState, _body);
	if (code == 0)
		return ;
	setFinished(code);
}
