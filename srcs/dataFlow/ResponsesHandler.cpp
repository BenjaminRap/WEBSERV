#include <exception>             // for exception
#include <queue>                 // for queue

#include "FlowBuffer.hpp"        // for FlowState, FlowBuffer (ptr only)
#include "RawResponse.hpp"       // for RawResponse
#include "Response.hpp"          // for Response
#include "ResponsesHandler.hpp"  // for ResponsesHandler, RESPONSE_BUFFER_SIZE

class ServerConfiguration;  // lines 10-10

/*************************Constructors / Destructors **************************/

ResponsesHandler::ResponsesHandler(const ServerConfiguration &defaultConfig) :
	_currentResponse(defaultConfig),
	_responses(),
	_buffer(),
	_responseBuffer(_buffer, RESPONSE_BUFFER_SIZE, 0)
{

}

ResponsesHandler::~ResponsesHandler()
{
	while (_responses.size() != 0)
	{
		const RawResponse*	rawResponse = _responses.front();
		delete rawResponse;
		_responses.pop();
	}
}

/***************************Member functions***********************************/

FlowState	ResponsesHandler::sendResponseToSocket(int socketFd)
{
	if (_responses.size() == 0)
		return (FLOW_DONE);

	RawResponse		*response = _responses.front();
	const FlowState	flowState = response->sendResponseToSocket(socketFd);

	if (flowState == FLOW_DONE)
	{
		_responses.pop();
		delete response;
		return ((_responses.size() == 0) ? FLOW_DONE : FLOW_MORE);
	}
	else
		return (flowState);
}

void		ResponsesHandler::addCurrentResponseToQueue()
{
	RawResponse*	rawResponse = new RawResponse(_currentResponse, _responseBuffer);
	try
	{
		_responses.push(rawResponse);
	}
	catch (std::exception& exception)
	{
		delete rawResponse;
	}
	_currentResponse.reset();
}


Response&	ResponsesHandler::getCurrentResponse()
{
	return (_currentResponse);
}

FlowBuffer&	ResponsesHandler::getFlowBuffer(void)
{
	return (_responseBuffer);
}
