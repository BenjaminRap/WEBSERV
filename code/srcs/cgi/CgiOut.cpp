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
	if (!(_isActive && events & EPOLLIN))
		return ;
	const FlowState flowState = _responseFlowBuffer.srcToBuff<int>(getFd());

	if (flowState == FLOW_ERROR || flowState == FLOW_DONE)
		_isActive = false;
}
