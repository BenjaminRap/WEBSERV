#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include <vector>

# include "FlowBuffer.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConfiguration.hpp"

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

class ARequestType;

class RequestHandler
{
private:
	char									_buffer[REQUEST_BUFFER_SIZE];
	FlowBuffer								_flowBuffer;
	RequestState							_state;
	Request									_request;
	const std::vector<ServerConfiguration>	&_serverConfs;

	RequestHandler(const RequestHandler& ref);

	RequestHandler&	operator=(const RequestHandler& ref);

	void						readStatusLine(Response &response);
	void						readHeaders(Response &response);
	void						executeRequest(Response &response, int socketFd);
	void						writeBodyFromBuffer(Response &response);

	const ServerConfiguration	&getServerConfiguration(void) const;
	void						processRequestResult(ARequestType& request, Response &response, int socketFd);
public:
	RequestHandler(const std::vector<ServerConfiguration> &serverConfs);
	~RequestHandler();

	RequestState				redirectBodySocketToFile(int socketFd, Response &response);
	RequestState				redirectSocketToBuffer(int socketFd, Response &response);

	RequestState				readRequest(Response &response, int socketFd);
	bool						isRequestBody(void);
	void						setNewRequest(void);
};

#endif // !REQUEST_HANDLER_HPP
