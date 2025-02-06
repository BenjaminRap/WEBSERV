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

void	removeConnectedSocket()
{
}
void	sendResponse()
{
}
void	resetValues()
{
}

RequestState	ConnectedSocketData::processRequest()
{
	RequestState	requestState;

	if (_requestHandler.isRequestBody())
	{
		requestState = _requestHandler.redirectBodySocketToFile(_fd);
	}
	else
	{
		requestState = _requestHandler.redirectSocketToBuffer(_fd);
		
		if (requestState != CONNECTION_CLOSED || requestState != REQUEST_DONE)
			requestState = _requestHandler.readRequest();
	}
	while  (requestState == REQUEST_DONE)
	{
		sendResponse();
		resetValues();
		requestState = _requestHandler.readRequest();
	}
	return (requestState);
}

void	ConnectedSocketData::callback(uint32_t events)
{
	if (events & EPOLLIN)
	{
		if (processRequest() == CONNECTION_CLOSED)
		{
			removeConnectedSocket();
			return ;
		}
	}
	if (events & EPOLLOUT)
	{
		_responsesHandler.sendResponsesToSocket(_fd);
	}
}