#include <sys/epoll.h>		// for EPOLLOUT

#include "CgiIn.hpp"		// for CgiIn
#include "FlowBuffer.hpp"	// for FlowBUffer

CgiIn::CgiIn
(
	int fd,
	SocketsHandler &socketsHandler,
	const std::vector<ServerConfiguration> &serverConfigurations,
	FlowBuffer& requestFlowBuffer
) :
	AFdData(fd, socketsHandler, serverConfigurations),
	_requestFlowBuffer(requestFlowBuffer)
{
}

CgiIn::~CgiIn()
{

}

void	CgiIn::callback(uint32_t events)
{
	if (!(events & EPOLLOUT)
		|| _requestFlowBuffer.isBufferEmpty()
		|| _body == NULL)
	{
		return ;
	}
	_requestFlowBuffer.redirectBufferContentToFd<ABody&>(*_body, ABody::callInstanceWriteToFd);
}
