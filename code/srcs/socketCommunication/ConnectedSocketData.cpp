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

void	ConnectedSocketData::callback(uint32_t events)
{
	if (events & EPOLLIN)
	{
		if (_requestHandler.isRequestBody())
		{
			
		}
		else
		{
			
		}
	}
	if (events & EPOLLOUT)
	{
		_responsesHandler.sendResponsesToSocket(_fd);
	}
}