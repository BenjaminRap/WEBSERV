#include <cstddef>               // for size_t
#include <queue>                 // for queue

#include "FlowBuffer.hpp"        // for FlowState
#include "RawResponse.hpp"       // for RawResponse
#include "ResponsesHandler.hpp"  // for ResponsesHandler, RESPONSE_BUFFER_SIZE

/*************************Constructors / Destructors **************************/

ResponsesHandler::ResponsesHandler() :
	_responses(),
	_buffer(),
	_responseBuffer(_buffer, RESPONSE_BUFFER_SIZE, 0)
{

}

ResponsesHandler::~ResponsesHandler()
{

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

	RawResponse		&response = _responses.front();
	const FlowState	flowState = response.sendResponseToSocket(socketFd);

	if (flowState == FLOW_DONE)
	{
		_responses.pop();
		return ((_responses.size() == 0) ? FLOW_DONE : FLOW_MORE);
	}
	else
		return (flowState);
}

void		ResponsesHandler::addCurrentResponseToQueue()
{
	//_responses.push(RawResponse(_currentResponse));
	_currentResponse.reset();
}


Response&	ResponsesHandler::getCurrentResponse()
{
	return (_currentResponse);
}