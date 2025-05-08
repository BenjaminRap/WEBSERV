#include <fcntl.h>                  // for O_RDONLY, O_WRONLY
#include <stdint.h>                 // for uint16_t, uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOL...
#include <cstdio>                   // for NULL, remove
#include <stdexcept>                // for runtime_error
#include <string>                   // for basic_string

#include "ABody.hpp"                // for ABody, ABodyChilds
#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "CgiIn.hpp"                // for CgiIn, CgiInState, CGI_IN_EVENTS
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "FileFd.hpp"               // for FileFd
#include "FlowBuffer.hpp"           // for FlowBuffer, FlowState
#include "RequestHandler.hpp"       // for RequestState
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
	Response& currentResponse,
	const int (&redirectFds)[2]
) :
	AFdData(fd, ePollHandler, CGI_IN, CGI_IN_EVENTS),
	_flowBuf(requestFlowBuffer),
	_body(body),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse),
	_tempName(),
	_tempFile(NULL),
	_state(BUF_TO_CGI),
	_redirectFds()
{
	_redirectFds[0] = redirectFds[0];
	_redirectFds[1] = redirectFds[1];
	if (_body.getType() != CHUNKED_REQUEST)
		return ;
	_tempName[0] = '\0';
	_tempFile = FileFd::getTemporaryFile(_tempName, O_WRONLY);
	if (_tempFile == NULL)
		throw std::runtime_error("error creating a temporary file !");
	_body.setFd(_tempFile->getFd());
	_state =  BUF_TO_TEMP;
}

CgiIn::~CgiIn()
{
	if (_tempFile != NULL)
	{
		std::remove(_tempName);
		delete(_tempFile);
	}
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body);


void	CgiIn::setFinished(uint16_t code)
{
	_isActive = false;
	_response.setResponse(code);
	_connectedSocketData.readNextRequests(_response, REQUEST_DONE);
}

void	CgiIn::redirectToTemp(void)
{
	const FlowState	flowState = _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);
	const uint16_t	code = getCodeIfFinished(true, flowState, _body);

	if (code == 0)
		return ;
	if (code == HTTP_OK)
	{
		_state = TEMP_TO_CGI;
		delete _tempFile;
		_tempFile = FileFd::getTemporaryFile(_tempName, O_RDONLY);
		if (_tempFile == NULL)
			setFinished(HTTP_INTERNAL_SERVER_ERROR);

	}
	else
		setFinished(code);
}

void	CgiIn::callback(uint32_t events)
{
	if (!_isActive)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
	{
		setFinished(HTTP_BAD_GATEWAY);
		return ;
	}
	if (_state == BUF_TO_TEMP)
	{
		redirectToTemp();
		return ;
	}
	if (!(events & EPOLLIN))
		return ;

	const FlowState	flowState = (TEMP_TO_CGI)
		? _flowBuf.redirect(_tempFile->getFd(), getFd())
		: _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);

	const uint16_t	code = getCodeIfFinished(true, flowState, _body);
	if (code == 0)
		return ;
	setFinished(code);
}
