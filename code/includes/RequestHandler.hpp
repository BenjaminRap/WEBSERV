#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include "FlowBuffer.hpp"
# include "Request.hpp"

# define REQUEST_BUFFER_SIZE 1024

enum	RequestState
{
	REQUEST_STATUS_LINE,
	REQUEST_HEADERS,
	REQUEST_EMPTY_LINE,
	REQUEST_BODY,
	REQUEST_DONE,
	CONNECTION_CLOSED
};

class RequestHandler
{
private:
	char			_buffer[REQUEST_BUFFER_SIZE];
	FlowBuffer		_flowBuffer;
	RequestState	_state;
	Request			_request;

	RequestHandler(const RequestHandler& ref);

	RequestHandler&	operator=(const RequestHandler& ref);

	void			readStatusLine();
	void			readHeaders();
	void			executeRequest();
	void			writeBodyFromBuffer();
public:
	RequestHandler();
	~RequestHandler();

	RequestState	redirectBodySocketToFile(int socketFd);
	RequestState	redirectSocketToBuffer(int socketFd);

	RequestState	readRequest();
	bool			isRequestBody(void);
};

#endif // !REQUEST_HANDLER_HPP