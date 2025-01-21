#include "ResponsesHandler.hpp"

/*************************Constructors / Destructors **************************/

ResponsesHandler::ResponsesHandler() :
	_responses(),
	_buffer(),
	_responseBuffer(_buffer, RESPONSE_BUFFER_SIZE),
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
ssize_t	ResponsesHandler::sendResponsesToSocket(int socketFd)
{
	if (_canWrite == false)
		return (1);
	while (_responses.size() != 0)
	{
		RawResponse		&response = _responses.front();
		const ssize_t	res = response.sendResponseToSocket(socketFd);

		if (res == -1)
			return (-1);
		if (res > 0)
		{
			_canWrite = false;
			return (res);
		}
		_responses.pop();
	}
	return (0);
}

/**
 * @brief Allow this class to write into a socket. It should only be called when
 * receiving EPOLLOUT. 
 */
void	ResponsesHandler::enableWritingToSocket()
{
	_canWrite = true;
}