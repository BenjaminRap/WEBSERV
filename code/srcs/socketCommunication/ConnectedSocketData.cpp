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
	(void)events;
}