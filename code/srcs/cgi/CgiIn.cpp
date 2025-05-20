#include <fcntl.h>                  // for O_RDONLY, O_WRONLY
#include <stdint.h>                 // for uint16_t, uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOL...
#include <cstdio>                   // for NULL, remove

#include "ABody.hpp"                // for ABody, ABodyChilds
#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "CgiIn.hpp"                // for CgiIn, CgiInState, CGI_IN_EVENTS
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "FlowBuffer.hpp"           // for FlowBuffer, FlowState
#include "Response.hpp"             // for Response
#include "requestStatusCode.hpp"    // for HTTP_BAD_GATEWAY, HTTP_INTERNAL_S...

class EPollHandler;

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
	if (!(events & EPOLLIN))
		return ;
	const FlowState	flowState = _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);

	const uint16_t	code = getCodeIfFinished(true, flowState, _body);
	if (code == 0)
		return ;
	setFinished(code);
}
