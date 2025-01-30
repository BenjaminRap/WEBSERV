#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include "FlowBuffer.hpp"

# define REQUEST_BUFFER_SIZE 1024

enum RequestState
{
	REQUEST_STATUS_LINE,
	REQUEST_HEADERS,
	REQUEST_BODY
};

class RequestHandler
{
private:
	char			_buffer[REQUEST_BUFFER_SIZE];
	FlowBuffer		_flowBuffer;
	RequestState	_state;

	RequestHandler&	operator=(const RequestHandler& ref);
public:
	RequestHandler();
	RequestHandler(const RequestHandler& ref);
	~RequestHandler();

	bool	readRequest(int socketFd);
	bool	processRequest(int socketFd);
};

#endif // !REQUEST_HANDLER_HPP