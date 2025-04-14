#include <stddef.h>               // for NULL

#include "ABody.hpp"              // for ABody
#include "FlowBuffer.hpp"         // for FlowState, FlowBuffer
#include "Request.hpp"            // for Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState
#include "Response.hpp"           // for Response
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR, HTTP_BA...


RequestState	RequestHandler::redirectBody(int socketFd, Response &response, bool canRead)
{
	if (_state != REQUEST_BODY)
		return (_state);
	ABody * const	body = _request.getBody();
	
	if (body == NULL)
	{
		_state = REQUEST_DONE;
		return (_state);
	}
	const bool	canWrite = _request.getIsBlocking() == false;

	FlowState	flowState;

	if (canRead && !canWrite)
		flowState = _flowBuffer.redirectFdContentToBuffer<int>(socketFd);
	else if (!canRead && canWrite)
		flowState = _flowBuffer.redirectBufferContentToFd<ABody&>(*body, ABody::writeToFd);
	else if (canRead && canWrite)
		_flowBuffer.redirectContent<int, ABody&>(socketFd, *body, ABody::writeToFd);
	else
		return (REQUEST_BODY);

	uint16_t code = HTTP_OK;

	if (body->getFinished())
	{
		_state = REQUEST_DONE;
		code = body->getStatus();
	}
	else if (flowState == FLOW_ERROR)
		code = HTTP_INTERNAL_SERVER_ERROR;
	else if (_request.getIsBlocking() == false && _flowBuffer.isBufferFull())
		code = HTTP_BAD_REQUEST;
	if (code != HTTP_OK)
	{
		_state = REQUEST_DONE;
		response.setResponse(code);
	}
	return (_state);
}

RequestState	RequestHandler::redirectFirstPart(int socketFd, Response &response)
{
	const FlowState flowState = _flowBuffer.redirectFdContentToBuffer<int>(socketFd);

	if (flowState == FLOW_ERROR)
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
