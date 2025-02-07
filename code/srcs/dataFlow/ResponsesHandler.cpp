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

/**
 * @brief Add a response at the end of the _responses queue. The response has a
 * body fd.
 * @throw This function throw (std::logic_error) if bufferLength is superior to
 * bufferCapacity, if the buffer is null or if the bufferCapacity is set to 0.
 * It can also throw (std::bad_alloc) if the push fail.
 * @param firstPart The first part of the response. It is composed by the status line,
 * the headers, the empty line and, maybe, a part of the body.
 * @param firstPartLength The length of firstPart.
 * @param bodyFd The fd of the body.
 */
void	ResponsesHandler::addResponse
(
	char *firstPart,
	std::size_t firstPartLength,
	int bodyFd
)
{
	_responses.push(RawResponse(firstPart, firstPartLength, bodyFd, _responseBuffer));
}

/**
 * @brief Add a response at the end of the _responses queue. The response doesn't
 * have a body fd.
 * @throw This function throw (std::logic_error) if bufferLength is superior to
 * bufferCapacity, if the buffer is null or if the bufferCapacity is set to 0.
 * It can also throw (std::bad_alloc) if the push fail.
 * @param firstPart The first part of the response. It is composed by the status line,
 * the headers, the empty line and, maybe, a part of the body.
 * @param firstPartLength The length of firstPart.
 */
void	ResponsesHandler::addResponse
(
	char *firstPart,
	std::size_t firstPartLength
)
{
	_responses.push(RawResponse(firstPart, firstPartLength));
}


Response&	ResponsesHandler::getCurrentResponse()
{
	return (_currentResponse);
}