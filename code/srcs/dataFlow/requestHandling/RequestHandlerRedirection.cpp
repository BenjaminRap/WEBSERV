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
	AFdData * const	fdData = _request.getFdData();
	
	if (body == NULL)
	{
		_state = REQUEST_DONE;
		return (REQUEST_DONE);
	}
	const bool	canWrite = _request.isBodyBlocking() == false;

	FlowState	flowState;

	if (canRead && !canWrite)
	{
		flowState = _flowBuf.srcToBuff<int>(socketFd);
		fdData->callback(0);
	}
	else if (!canRead && canWrite)
		flowState = _flowBuf.buffToDest<ABody&>(*body, ABody::writeToFd);
	else if (canRead && canWrite)
		_flowBuf.redirect<int, ABody&>(socketFd, *body, ABody::writeToFd);
	else
		return (REQUEST_BODY);

	int	code;

	if (canWrite && body->getFinished())
		code = body->getStatus();
	else if (flowState == FLOW_ERROR)
		code = HTTP_INTERNAL_SERVER_ERROR;
	else if (canWrite && flowState == FLOW_BUFFER_FULL)
		code = HTTP_BAD_REQUEST;
	else
		return (_state);

	if (code != HTTP_OK)
		response.setResponse(code);
	_state = REQUEST_DONE;
	return (REQUEST_DONE);
}

RequestState	RequestHandler::redirectFirstPart(int socketFd, Response &response)
{
	const FlowState flowState = _flowBuf.srcToBuff<int>(socketFd);

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
