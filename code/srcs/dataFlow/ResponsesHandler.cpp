#include <iostream>              // for char_traits, basic_ostream, operator<<
#include <queue>                 // for queue

#include "FlowBuffer.hpp"        // for FlowState
#include "RawResponse.hpp"       // for RawResponse
#include "Response.hpp"          // for Response
#include "ResponsesHandler.hpp"  // for ResponsesHandler, RESPONSE_BUFFER_SIZE

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

/**
 * @brief Send all the responses to the client socket.
 * @param socketFd the file descriptor to the client socket.
 * @return FLOW_ERROR on error, FLOW_DONE if all responses has been entirely
 * written, FLOW_MORE if there is more to send. In the latter case,
 * we should wait for an EPOLLOUT and call this function again, until we
 * receive a FLOR_ERROR or FLOW_DONE.
 */
FlowState	ResponsesHandler::sendResponsesToSocket(int socketFd)
{
	if (_responses.size() == 0)
		return (FLOW_DONE);

	RawResponse		*response = _responses.front();
	const FlowState	flowState = response->sendResponseToSocket(socketFd);

	if (flowState == FLOW_DONE)
	{
		std::cout << "Raw Response written !" << std::endl;
		_responses.pop();
		delete response;
		return ((_responses.size() == 0) ? FLOW_DONE : FLOW_MORE);
	}
	else
		return (flowState);
}

void		ResponsesHandler::addCurrentResponseToQueue()
{
	_responses.push(new RawResponse(_currentResponse, _responseBuffer));
	std::cout << "Add response to queue : \n" << _currentResponse << std::endl;
	_currentResponse.reset();
}


Response&	ResponsesHandler::getCurrentResponse()
{
	return (_currentResponse);
}
