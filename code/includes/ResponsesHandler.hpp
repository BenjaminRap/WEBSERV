#ifndef RESPONSES_HANDLER_HPP
# define RESPONSES_HANDLER_HPP

# include <queue>

# include "RawResponse.hpp"
# include "FlowBuffer.hpp"

# define RESPONSE_BUFFER_SIZE 1024

/**
 * @brief This class manages the responses that a connection client/server needs
 * to send. The responses are sent in the same order that the corresponding request
 * has been received.
 * It uses a queue of RawResponse.
 */
class ResponsesHandler
{
private:
	/**
	 * @brief A queue of all the responses. They will be written to the client in
	 * the same order that the client send the requests.
	 */
	std::queue<RawResponse>	_responses;
	/**
	 * @brief The buffer that will be passed in the responseBuffer constructor.
	 * It will be used to store parrts of the response body before sending it to
	 * the client.
	 * As we send one response at a time, we only need one buffer for all responses.
	 */
	char					_buffer[RESPONSE_BUFFER_SIZE];
	/**
	 * @brief This instance store the _buffer and informations about it. It also
	 * allows to redirect the data from the body to the socket like a stream.
	 */
	FlowBuffer				_responseBuffer;
	/**
	 * @brief True if we can write in the client socket. It is set true when we
	 * get a EPOLLOUT, but false when there is a EAGAIN.
	 */
	bool					_canWrite;

	ResponsesHandler(const ResponsesHandler& ref);

	ResponsesHandler&	operator=(const ResponsesHandler& ref);
public:
	ResponsesHandler();
	~ResponsesHandler();

	ssize_t	sendResponsesToSocket(int socketFd);
	void	enableWritingToSocket();
	void	addResponse(char *buffer, std::size_t bufferCapacity, int bodyFd);
};

#endif // !RESPONSES_HANDLER_HPP