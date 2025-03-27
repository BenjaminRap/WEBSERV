#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLIN, EPOLLOUT
#include <vector>                   // for vector

#include "AFdData.hpp"              // for AFdData
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "RequestHandler.hpp"       // for RequestHandler, RequestState
#include "ResponsesHandler.hpp"     // for ResponsesHandler
#include "SocketsHandler.hpp"		// for SocketsHandler

class Response;
class ServerConfiguration;
class SocketsHandler;

/*************************Constructors / Destructors***************************/

ConnectedSocketData::ConnectedSocketData(int fd, SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> &serverConfiguration) :
	AFdData(fd, socketsHandler, serverConfiguration),
	_responsesHandler(serverConfiguration.front()),
	_requestHandler(serverConfiguration)
{

}

ConnectedSocketData::~ConnectedSocketData(void)
{
	std::cout << "ConnectedSocketData removed" << std::endl;
}

/***************************Member functions***********************************/

RequestState	ConnectedSocketData::processRequest(Response &response)
{
	RequestState	requestState;

	if (_requestHandler.isRequestBody())
	{
		requestState = _requestHandler.redirectBodySocketToFile(_fd, response);
	}
	else
	{
		requestState = _requestHandler.redirectSocketToBuffer(_fd, response);
		
		if (requestState != CONNECTION_CLOSED && requestState != REQUEST_DONE)
			requestState = _requestHandler.readRequest(response, _fd);
	}
	while  (requestState == REQUEST_DONE)
	{
		_responsesHandler.addCurrentResponseToQueue();
		_requestHandler.setNewRequest();
		requestState = _requestHandler.readRequest(response, _fd);
	}
	return (requestState);
}

void	ConnectedSocketData::callback(uint32_t events)
{
	Response	&response = _responsesHandler.getCurrentResponse();
	bool		closing = false;

	try
	{
		if (events & EPOLLIN)
		{
			if (processRequest(response) == CONNECTION_CLOSED)
				closing = true;
		}
		if (closing == false && events & EPOLLOUT)
		{
			if (_responsesHandler.sendResponsesToSocket(_fd) == FLOW_ERROR)
				closing = true;
		}
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		closing = true;
	}
	if (closing)
		_socketsHandler.removeSocketFromList(_iterator);
}
