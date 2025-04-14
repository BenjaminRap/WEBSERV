#include <sys/epoll.h>		// for EPOLLOUT

#include "CgiIn.hpp"		// for CgiIn
#include "FlowBuffer.hpp"	// for FlowBUffer
#include "ABody.hpp"		// for ABody

CgiIn::CgiIn
(
	int fd,
	SocketsHandler &socketsHandler,
	const std::vector<ServerConfiguration> &serverConfigurations,
	FlowBuffer& requestFlowBuffer,
	ABody& body
) :
	AFdData(fd, socketsHandler, serverConfigurations),
	_requestFlowBuffer(requestFlowBuffer),
	_body(body)
{
}

CgiIn::~CgiIn()
{

}

/**
 * @brief 
 * @note We consider that the cgin has a body, or else this class is useless
 *
 * @param events 
 */
void	CgiIn::callback(uint32_t events)
{
	if (!(events & EPOLLOUT)
		|| _requestFlowBuffer.isBufferEmpty())
	{
		return ;
	}
	_requestFlowBuffer.redirectBufferContentToFd<ABody&>(_body, ABody::callInstanceWriteToFd);
}
