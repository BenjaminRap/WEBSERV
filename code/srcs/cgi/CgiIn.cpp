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
	AFdData(fd, ePollHandler),
	_requestFlowBuffer(requestFlowBuffer),
	_body(body),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse),
	_tempFile(NULL),
	_tempFileSize(-1)
{
	if (dynamic_cast<ChunkedBody*>(&_body) == NULL)
		return ;
	_tempFile = std::tmpfile();
	if (_tempFile == NULL)
		throw std::runtime_error("error creating a temporary file !");
}

CgiIn::~CgiIn()
{
	if (_tempFile != NULL)
		fclose(_tempFile);
}

void	CgiIn::callback(uint32_t events)
{
	(void)events;
	if (!_isActive)
		return ;
	if (_tempFile != NULL)
	{
	}
		return ;
	const FlowState	flowState = _requestFlowBuffer.
		redirectBufferContentToFd<ABody&>(_body, ABody::writeToFd);

	int	code;

	if (_body.getFinished())
		code = _body.getStatus();
	else if (flowState == FLOW_ERROR)
		code = HTTP_INTERNAL_SERVER_ERROR;
	else if (flowState == FLOW_BUFFER_FULL)
		code = HTTP_BAD_REQUEST;
	else
		return ;
	if (code != HTTP_OK)
		_response.setResponse(code);
	_isActive = false;
	_connectedSocketData.readNextRequests(_response, REQUEST_DONE); // this will destroy this instance
}
