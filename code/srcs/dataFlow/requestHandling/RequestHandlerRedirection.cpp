#include "RequestHandler.hpp"

void	RequestHandler::writeBodyFromBuffer(Response &response)
{
	if (_state != REQUEST_BODY)
		return ;
	Body * const	body = _request.getBody();

	if (body == NULL)
	{
		_state = REQUEST_DONE;
		return ;
	}
	if (body->getIsBlocking())
		return ;
	const FlowState flowState = _flowBuffer.redirectBufferContentToFd<Body&>(*body, Body::callInstanceWriteToFd);
	
	if (flowState == FLOW_ERROR)
	{
		response.setResponse(500, "");
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_DONE && body->getFinished())
		_state = REQUEST_DONE;
}

/*************************public Member function*******************************/

RequestState			RequestHandler::redirectBodySocketToFile(int socketFd, Response &response)
{
	Body * const	body = _request.getBody();
	
	if (body == NULL)
		return (REQUEST_DONE);
	const FlowState flowState = body->getIsBlocking() ?
		_flowBuffer.redirectFdContentToBuffer<int>(socketFd)
		: _flowBuffer.redirectContent<int, Body&>(socketFd, *body, Body::callInstanceWriteToFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(500, "");
		_state = REQUEST_DONE;
	}
	else if (body->getFinished())
		_state = REQUEST_DONE;
	return (_state);
}

RequestState	RequestHandler::redirectSocketToBuffer(int socketFd, Response &response)
{
	const FlowState flowState = _flowBuffer.redirectFdContentToBuffer<int>(socketFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(500, "");
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_BUFFER_FULL)
	{
		response.setResponse(400, "");
		_state = REQUEST_DONE;
	}
	return (_state);
}


