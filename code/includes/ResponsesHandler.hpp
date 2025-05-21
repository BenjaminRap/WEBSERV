#ifndef RESPONSES_HANDLER_HPP
# define RESPONSES_HANDLER_HPP

# include <queue>

# include "RawResponse.hpp"
# include "FlowBuffer.hpp"
# include "Response.hpp"

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
	Response				_currentResponse;
	/**
	 * @brief A queue of all the responses. They will be written to the client in
	 * the same order that the client send the requests.
	 */
	std::queue<RawResponse *>	_responses;
	/**
	 * @brief The buffer that will be passed in the responseBuffer constructor.
	 * It will be used to store parts of the response body before sending it to
	 * the client.
	 * As we send one response at a time, we only need one buffer for all responses.
	 */
	char					_buffer[RESPONSE_BUFFER_SIZE];
	/**
	 * @brief This instance store the _buffer and informations about it. It also
	 * allows to redirect the data from the body to the socket like a stream.
	 */
	FlowBuffer				_responseBuffer;

	ResponsesHandler();
	ResponsesHandler(const ResponsesHandler& ref);

	ResponsesHandler&	operator=(const ResponsesHandler& ref);
public:
	ResponsesHandler(const ServerConfiguration &defaultConfig);
	~ResponsesHandler();

	/**
	 * @brief Send a response, or a part of it to the client socket.
	 * @param socketFd the file descriptor to the client socket.
	 * @return FLOW_ERROR on error, FLOW_DONE if all responses has been entirely
	 * written, FLOW_MORE if there is more to send. In the latter case,
	 * we should wait for an EPOLLOUT and call this function again, until we
	 * receive a FLOR_ERROR or FLOW_DONE.
	 */
	FlowState	sendResponseToSocket(int socketFd);
	/**
	 * @brief Convert the _currentResponse to a RawResponse, adds it to
	 * the queue and reset it.
	 */
	void		addCurrentResponseToQueue();
	Response&	getCurrentResponse();
	FlowBuffer&	getFlowBuffer(void);
};

#endif // !RESPONSES_HANDLER_HPP
