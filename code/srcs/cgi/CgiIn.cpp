#include <sys/epoll.h>				// for EPOLLOUT

#include "CgiIn.hpp"				// for CgiIn
#include "ChunkedBody.hpp"			// for ChunkedBody
#include "FlowBuffer.hpp"			// for FlowBUffer
#include "ABody.hpp"				// for ABody
#include "ConnectedSocketData.hpp"	// for ConnectedSocketData
#include "Response.hpp"				// for Response
#include "requestStatusCode.hpp"	// for HTTP_...

CgiIn::CgiIn
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer& requestFlowBuffer,
	ABody& body,
	ConnectedSocketData& connectedSocketData,
	Response& currentResponse
) :
	AFdData(fd, ePollHandler, CGIIN),
	_flowBuf(requestFlowBuffer),
	_body(body),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse),
	_tempName(),
	_tempFile(NULL),
	_state(BUF_TO_CGI)
{
	_tempName[0] = '\0';
	if (dynamic_cast<ChunkedBody*>(&_body) == NULL)
		return ;
	if (!std::tmpnam(_tempName))
		throw std::runtime_error("error creating a temporary file !");
	_tempFile = new FileFd(_tempName, O_RDONLY | O_CREAT | O_EXCL);
	_state =  BUF_TO_TEMP;
}

CgiIn::~CgiIn()
{
	if (_tempFile != NULL)
		delete(_tempFile);
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body);


void	CgiIn::setFinished(uint16_t code)
{
	_response.setResponse(code);
	_connectedSocketData.readNextRequests(_response, REQUEST_DONE);
	_isActive = false;
}

void	CgiIn::redirectToTemp(void)
{
	const FlowState	flowState = _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);
	const uint16_t	code = getCodeIfFinished(true, flowState, _body);

	if (code == 0)
		return ;
	if (code == HTTP_OK)
	{
		delete _tempFile;
		_tempFile = new FileFd(_tempName, O_WRONLY);
		_state = TEMP_TO_CGI;
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
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
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
