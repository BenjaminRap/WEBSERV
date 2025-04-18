#include <stddef.h>               // for NULL

#include "ABody.hpp"              // for ABody
#include "FlowBuffer.hpp"         // for FlowState, FlowBuffer
#include "Request.hpp"            // for Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState
#include "Response.hpp"           // for Response
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR, HTTP_BA...

void	RequestHandler::writeBodyFromBuffer(Response &response)
{
	if (_state != REQUEST_BODY)
		return ;
	ABody * const	body = _request.getBody();

	if (body == NULL)
	{
		_state = REQUEST_DONE;
		return ;
	}
	if (_request.getIsBlocking())
		return ;
	const FlowState flowState = _flowBuffer.redirectBufferContentToFd<ABody&>(*body, ABody::callInstanceWriteToFd);
	
	if (flowState == FLOW_ERROR)
	{
		response.setResponse(HTTP_INTERNAL_SERVER_ERROR);
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_DONE && body->getFinished())
		_state = REQUEST_DONE;
}

/*************************public Member function*******************************/

RequestState			RequestHandler::redirectBody(int socketFd, Response &response)
{
	if (_state != REQUEST_BODY)
		return (_state);
	ABody * const	body = _request.getBody();
	
	if (body == NULL)
		return (REQUEST_DONE);
	const FlowState flowState = _request.getIsBlocking() ?
		_flowBuffer.redirectFdContentToBuffer<int>(socketFd)
		: _flowBuffer.redirectContent<int, ABody&>(socketFd, *body, ABody::callInstanceWriteToFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(HTTP_INTERNAL_SERVER_ERROR);
		_state = REQUEST_DONE;
	}
	else if (body->getFinished())
		_state = REQUEST_DONE;
	return (_state);
}

RequestState	RequestHandler::redirectFirstPart(int socketFd, Response &response)
{
	const FlowState flowState = _flowBuffer.redirectFdContentToBuffer<int>(socketFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(HTTP_INTERNAL_SERVER_ERROR);
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_BUFFER_FULL)
	{
		if (_state == REQUEST_HEADERS)
			response.setResponse(HTTP_REQUEST_HEADER_FIELDS_TOO_LARGE);
		else
			response.setResponse(HTTP_BAD_REQUEST);
		_state = REQUEST_DONE;
	}
	return (_state);
}


