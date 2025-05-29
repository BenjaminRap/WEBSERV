#include <stdint.h>                 // for uint16_t, uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN

#include "ABody.hpp"                // for ABody
#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "CgiIn.hpp"                // for CgiIn, CGI_IN_EVENTS
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "FlowBuffer.hpp"           // for FlowState, FlowBuffer
#include "Response.hpp"             // for Response

class EPollHandler;  // lines 14-14

CgiIn::CgiIn
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer& requestFlowBuffer,
	ABody& body,
	ConnectedSocketData& connectedSocketData,
	Response& currentResponse
) :
	AFdData(fd, ePollHandler, CGI_IN, CGI_IN_EVENTS),
	_flowBuf(requestFlowBuffer),
	_body(body),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse)
{
}

CgiIn::~CgiIn()
{
}

void	CgiIn::setFinished(uint16_t code)
{
	_isActive = false;
	if (code != 0)
		_response.setResponse(code);
	_connectedSocketData.ignoreBodyAndReadRequests(_response);
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body);

void	CgiIn::callback(uint32_t events)
{
	if (!_isActive)
		return ;
	if (events & (EPOLLERR | EPOLLHUP))
	{
		setFinished(0);
		return ;
	}
	checkTime();
	if (!(events & EPOLLOUT))
		return ;
	const FlowState	flowState = _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);

	const uint16_t	code = getCodeIfFinished(true, flowState, _body);
	if (code == 0)
		return ;
	setFinished(code);
}

void			CgiIn::checkTime(void)
{
	time_t	now = time(NULL);
	if (difftime(now, _lastEPollIn) > TIMEOUT_VALUE_SEC || difftime(now, _lastEpollOut) > TIMEOUT_VALUE_SEC)
	{
		setFinished(408);
	}
}
