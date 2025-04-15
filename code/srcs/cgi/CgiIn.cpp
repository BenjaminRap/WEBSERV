#include <sys/epoll.h>				// for EPOLLOUT

#include "CgiIn.hpp"				// for CgiIn
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
	AFdData(fd, true, ePollHandler),
	_requestFlowBuffer(requestFlowBuffer),
	_body(body),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse)
{
}

CgiIn::~CgiIn()
{
}

/**
 * @brief 
 * @note We consider that the cgin has a body, or else this class is useless
 *
 * @param events 
 */
void	CgiIn::callback(uint32_t events)
{
	if (!(events & EPOLLOUT)
		|| _requestFlowBuffer.isBufferEmpty())
	{
		return ;
	}
	const FlowState	flowState = _requestFlowBuffer.
		redirectBufferContentToFd<ABody&>(_body, ABody::writeToFd);

	int	code;

	if (_body.getFinished())
		code = _body.getStatus();
	else if (flowState == FLOW_ERROR)
		code = HTTP_INTERNAL_SERVER_ERROR;
	else if (_requestFlowBuffer.isBufferFull())
		code = HTTP_BAD_REQUEST;
	else
		return ;
	if (code != HTTP_OK)
		_response.setResponse(code);
	_connectedSocketData.readNextRequests(_response, REQUEST_DONE);
}
