#ifndef RESPONSES_HANDLER_HPP
# define RESPONSES_HANDLER_HPP

# include <queue>

# include "RawResponse.hpp"

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
	std::queue<RawResponse>			_responses;
	/**
	 * @brief When writing the response body, we need a buffer to store what has
	 * been read from the body but not sent.
	 * As we read one response at a time, we only need one buffer for all
	 * responses.
	 */
	char							_buffer[RESPONSE_BUFFER_SIZE];
};

#endif // !RESPONSES_HANDLER_HPP