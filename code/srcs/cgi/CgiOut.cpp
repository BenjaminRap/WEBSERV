#include <sys/epoll.h>				// for EPOLLIN

#include "CgiOut.hpp"				// for CgiOut
#include "FlowBuffer.hpp"			// for FlowBUffer
#include "requestStatusCode.hpp"	// for HTTP_...

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer&	responseFlowBuffer
) :
	AFdData(fd, ePollHandler),
	_responseFlowBuffer(responseFlowBuffer),
	_firstPart(),
	_charsWritten(0),
	_headers(),
	_tempFile(NULL),
	_state(READ_HEADER),
	_code(0)
{
}

CgiOut::~CgiOut()
{

}

void	CgiOut::callback(uint32_t events)
{
	if (!_isActive)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
	{
		if (_state == READ_HEADER)
			_code = HTTP_BAD_GATEWAY;
		else if (_state == CGI_TO_TEMP)
			_state = TEMP_TO_BUFFER;
		else
			_isActive = false;
		return ;
	}
	const FlowState flowState = _responseFlowBuffer.srcToBuff<int>(getFd());

	if (flowState == FLOW_ERROR || flowState == FLOW_DONE)
	{
		_isActive = false;
		return ;
	}
}
