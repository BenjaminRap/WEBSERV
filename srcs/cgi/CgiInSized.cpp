#include <stdint.h>                 // for uint16_t, uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLOUT
#include <time.h>                   // for difftime, time_t
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...

#include "ABody.hpp"                // for ABody
#include "AEPollFd.hpp"             // for AEPollFd
#include "AFdData.hpp"              // for AFdData
#include "CgiInSized.hpp"           // for CgiInSized, CGI_IN_EVENTS
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "FlowBuffer.hpp"           // for FlowState, FlowBuffer
#include "Response.hpp"             // for Response
#include "SizedBody.hpp"            // for SizedBody
#include "protocol.hpp"             // for TIMEOUT
#include "requestStatusCode.hpp"    // for HTTP_GATEWAY_TIMEOUT, HTTP_INTERN...

class EPollHandler;  // lines 15-15

CgiInSized::CgiInSized
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer& requestFlowBuffer,
	SizedBody& sizedBody,
	ConnectedSocketData& connectedSocketData,
	Response& currentResponse
) :
	AEPollFd(fd, ePollHandler, CGI_IN_SIZED, CGI_IN_EVENTS),
	_requestBuf(requestFlowBuffer),
	_body(sizedBody),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse)
{
}

CgiInSized::~CgiInSized()
{
}

void	CgiInSized::setFinished(uint16_t code)
{
	try
	{
		AFdData::setFinished();
		if (code != 0)
			_response.setResponse(code);
		_connectedSocketData.ignoreBodyAndReadRequests(_response);
	}
	catch (const std::exception& exception)
	{
		std::cerr << "can't print response, removing the connected fd !\n";
		std::cerr << exception.what() << std::endl;
		AEPollFd::removeFromEPollHandler(&_connectedSocketData);
		AFdData::setFinished();
	}
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body);

void	CgiInSized::callback(uint32_t events)
{
	try
	{
		setTime(events);
		if (!getIsActive())
			return ;
		if (events & (EPOLLERR | EPOLLHUP))
		{
			setFinished(0);
			return ;
		}
		if (!(events & EPOLLOUT))
			return ;
		const FlowState	flowState = _requestBuf.buffToDest<ABody&>(_body, ABody::writeToFd);
		const uint16_t	code = getCodeIfFinished(true, flowState, _body);

		if (code != 0)
			setFinished(code);
	}
	catch (const std::exception& exception)
	{
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
	}
}

void			CgiInSized::checkTime(time_t now)
{
	if (!getIsActive())
		return ;

	if (difftime(now, _lastEpollOutTime) > TIMEOUT)
	{
		setFinished(HTTP_GATEWAY_TIMEOUT);
	}
}
