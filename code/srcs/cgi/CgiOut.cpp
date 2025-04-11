#include <sys/epoll.h>		// for EPOLLIN

#include "CgiOut.hpp"		// for CgiOut
#include "FlowBuffer.hpp"	// for FlowBUffer
#include "ABody.hpp"		// for ABody

CgiOut::CgiOut
(
	int fd,
	SocketsHandler &socketsHandler,
	const std::vector<ServerConfiguration> &serverConfigurations,
	FlowBuffer&	responseFlowBuffer,
	ABody* body
) :
	AFdData(fd, socketsHandler, serverConfigurations),
	_responseFlowBuffer(responseFlowBuffer),
	_body(body)
{
}

CgiOut::~CgiOut()
{

}

void	CgiOut::callback(uint32_t events)
{
	if (!(events & EPOLLIN)
		|| _responseFlowBuffer.isBufferFull()
		|| _body == NULL)
	{
		return ;
	}
}
