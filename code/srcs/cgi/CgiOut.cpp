#include <sys/epoll.h>		// for EPOLLIN

#include "CgiOut.hpp"		// for CgiOut
#include "FlowBuffer.hpp"	// for FlowBUffer

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer&	responseFlowBuffer
) :
	AFdData(fd, ePollHandler),
	_responseFlowBuffer(responseFlowBuffer)
{
}

CgiOut::~CgiOut()
{

}

void	CgiOut::callback(uint32_t events)
{
	if (!(events & EPOLLIN)
		|| _responseFlowBuffer.isBufferFull())
	{
		return ;
	}
	_responseFlowBuffer.redirectFdContentToBuffer<int>(getFd());
}
