#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include "FlowBuffer.hpp"

# define REQUEST_BUFFER_SIZE 1024

enum	RequestState
{
	REQUEST_STATUS_LINE,
	REQUEST_HEADERS,
	REQUEST_EMPTY_LINE,
	REQUEST_BODY
};

class RequestHandler
{
private:
	char			_buffer[REQUEST_BUFFER_SIZE];
	FlowBuffer		_flowBuffer;
	RequestState	_state;

	RequestHandler(const RequestHandler& ref);

	RequestHandler&	operator=(const RequestHandler& ref);
public:
	RequestHandler();
	~RequestHandler();

	FlowState	processRequest(int socketFd);
	int			readStatusLine();
	int			readHeaders();
	int			executeRequest();
	int			readBody();
};

#endif // !REQUEST_HANDLER_HPP