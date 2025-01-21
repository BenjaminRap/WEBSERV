#include "ResponsesHandler.hpp"

/*************************Constructors / Destructors **************************/

ResponsesHandler::ResponsesHandler() :
	_responses(),
	_buffer(),
	_responseBuffer(_buffer, RESPONSE_BUFFER_SIZE, 0),
	_canWrite(false)
{

}

ResponsesHandler::~ResponsesHandler()
{

}

/***************************Member functions***********************************/

/**
 * @brief Send all the responses to the client socket.
 * @param the file descriptor to the client socket.
 * @return -1 on error, 0 if all responses has been entirely written, >0 if
 * there is more to write. In the latter case, we should wait for an EPOLLOUT and
 * call this function again, until we receive a 0.
 */
FlowState	ResponsesHandler::sendResponsesToSocket(int socketFd)
{
	if (_canWrite == false)
		return (FLOW_MORE_WRITE);
	while (_responses.size() != 0)
	{
		RawResponse		&response = _responses.front();
		const FlowState	flowState = response.sendResponseToSocket(socketFd);

		if (flowState == FLOW_ERROR)
			return (flowState);
		if (flowState == FLOW_DONE)
			_responses.pop();
		else
		{
			_canWrite = false;
			return (flowState);
		}
	}
	return (FLOW_DONE);
}

/**
 * @brief Allow this class to write into a socket. It should only be called when
 * receiving EPOLLOUT. 
 */
void	ResponsesHandler::enableWritingToSocket()
{
	_canWrite = true;
}

/**
 * @brief Add a response at the end of the _responses queue. The response has a
 * body fd.
 */
void	ResponsesHandler::addResponse
(
	char *buffer,
	std::size_t bufferCapacity,
	int bodyFd
)
{
	_responses.push(RawResponse(buffer, bufferCapacity, bodyFd, _responseBuffer));
}

/**
 * @brief Add a response at the end of the _responses queue. The response doesn't
 * have a body fd.
 */
void	ResponsesHandler::addResponse
(
	char *buffer,
	std::size_t bufferCapacity
)
{
	_responses.push(RawResponse(buffer, bufferCapacity, _responseBuffer));
}