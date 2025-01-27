#include <iostream>

#include "ResponsesHandler.hpp"

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
 */
bool	ResponsesHandler::addResponse
(
	char *buffer,
	std::size_t bufferCapacity,
	int bodyFd
)
{
	try
	{
		_responses.push(RawResponse(buffer, bufferCapacity, bodyFd, _responseBuffer));
		return (true);
	}
	catch(const std::exception& e)
	{
		std::cerr << "fail to write a response : " << e.what() << '\n';
		return (false);
	}
}

/**
 * @brief Add a response at the end of the _responses queue. The response doesn't
 * have a body fd.
 */
bool	ResponsesHandler::addResponse
(
	char *buffer,
	std::size_t bufferCapacity
)
{
	try
	{
		_responses.push(RawResponse(buffer, bufferCapacity));
		return (true);
	}
	catch(const std::exception& e)
	{
		std::cerr << "fail to write a response : " << e.what() << '\n';
		return (false);
	}
	
}