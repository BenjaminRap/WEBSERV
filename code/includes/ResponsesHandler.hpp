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
	char					_buffer[RESPONSE_BUFFER_SIZE];
	FlowBuffer				_responseBuffer;

	ResponsesHandler(const ResponsesHandler& ref);

	ResponsesHandler&	operator=(const ResponsesHandler& ref);
public:
	ResponsesHandler();
	~ResponsesHandler();

};

#endif // !RESPONSES_HANDLER_HPP