#include <stddef.h>               // for NULL

#include "ABody.hpp"              // for ABody
#include "FlowBuffer.hpp"         // for FlowState, FlowBuffer
#include "Request.hpp"            // for Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState
#include "Response.hpp"           // for Response
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR, HTTP_BA...

static FlowState	redirect
(
	int srcFd,
	ABody& destBody,
	AFdData& fdData,
	bool canRead,
	bool canWrite,
	FlowBuffer &flowBuf
)
{
	FlowState	flowState;

	if (canRead && !canWrite)
		flowState = flowBuf.srcToBuff<int>(srcFd);
	else if (!canRead && canWrite)
		flowState = flowBuf.buffToDest<ABody&>(destBody, ABody::writeToFd);
	else if (canRead && canWrite)
		flowState = flowBuf.redirect<int, ABody&>(srcFd, destBody, ABody::writeToFd);
	else
		flowState = FLOW_MORE;
	fdData.callback(0);
	return (flowState);
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body)
{

	if (canWrite)
	{
		if (body.getFinished())
			return (body.getStatus());
		if (flowResult == FLOW_BUFFER_FULL)
			return (HTTP_BAD_REQUEST);
	}
	if (flowResult == FLOW_ERROR)
		return (HTTP_INTERNAL_SERVER_ERROR);
	return (0);
}

RequestState	RequestHandler::redirectBody(int socketFd, Response &response, bool canRead)
{
	if (_state != REQUEST_BODY)
		return (_state);
	ABody * const	body = _request.getBody();
	AFdData * const	fdData = _request.getFdData();
	
	if (body == NULL || fdData == NULL)
	{
		_state = REQUEST_DONE;
		return (REQUEST_DONE);
	}

	const bool		canWrite = fdData->getIsBlocking() == false;
	const FlowState	flowState = redirect(socketFd, *body, *fdData, canRead, canWrite, _flowBuf);
	const uint16_t	code = getCodeIfFinished(canWrite, flowState, *body);

	if (code == 0)
		return (REQUEST_BODY);
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
