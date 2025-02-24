#include <sys/epoll.h>

#include "ConnectedSocketData.hpp"

/*************************Constructors / Destructors***************************/

ConnectedSocketData::ConnectedSocketData(int fd, SocketsHandler &socketsHandler) :
	FdData(fd, socketsHandler),
	_responsesHandler(),
	_requestHandler()
{

}

ConnectedSocketData::~ConnectedSocketData(void)
{

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
			requestState = _requestHandler.readRequest(response);
	}
	while  (requestState == REQUEST_DONE)
	{
		_responsesHandler.addCurrentResponseToQueue();
		_requestHandler.setNewRequest();
		requestState = _requestHandler.readRequest(response);
	}
	return (requestState);
}

void	ConnectedSocketData::callback(uint32_t events)
{
	Response	&response = _responsesHandler.getCurrentResponse();

	if (events & EPOLLIN)
	{
		if (processRequest(response) == CONNECTION_CLOSED)
			return ;
	}
	if (events & EPOLLOUT)
	{
		_responsesHandler.sendResponsesToSocket(_fd);
	}
}
