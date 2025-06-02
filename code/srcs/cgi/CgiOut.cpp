#include <stdint.h>               // for uint32_t
#include <sys/epoll.h>            // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOLLR...
#include <sys/types.h>            // for pid_t
#include <cstdio>                 // for remove, NULL

#include "AEPollFd.hpp"           // for AEPollFd
#include "AFdData.hpp"            // for AFdData
#include "CgiOut.hpp"             // for CgiOut, CGI_OUT_EVENTS
#include "CgiOutArgs.hpp"         // for CgiOutArgs
#include "requestStatusCode.hpp"  // for HTTP_BAD_GATEWAY

class EPollHandler;  // lines 12-12

int	getCGIStatus(pid_t pid);

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	pid_t pid,
	const CgiOutArgs& cgiOutArgs
) :
	AEPollFd(fd, ePollHandler, CGI_OUT, CGI_OUT_EVENTS),
	_flowBuf(cgiOutArgs._responsesFlowBuffer),
	_firstPart(),
	_charsWritten(0),
	_headers(),
	_tempName(),
	_srcFile(NULL),
	_state(CgiOut::READ_HEADER),
	_code(0),
	_error(false),
	_serverConf(cgiOutArgs._serverConfiguration),
	_canWrite(false),
	_cgiReadFinished(false),
	_pid(pid),
	_addHeader(cgiOutArgs._addHeader)
{
	_tempName[0] = '\0';
}

CgiOut::~CgiOut()
{
	if (_tempName[0] != '\0')
		std::remove(_tempName);
	delete _srcFile;
	getCGIStatus(_pid);
}

void	CgiOut::setFinished(void)
{
	AFdData::setFinished();
	_state = CgiOut::DONE;
}

void	CgiOut::handleCgiError(uint16_t code)
{
	if (_state == CgiOut::READ_HEADER || _state == CgiOut::CGI_TO_TEMP)
	{
		_code = code;
		_error = true;
		generateFirstPart();
	}
	else
		setFinished();
}

bool	CgiOut::isResponseReady(void) const
{
	if (_state == CgiOut::READ_HEADER || _state == CgiOut::CGI_TO_TEMP)
		return (false);
	return (true);
}

void	CgiOut::callback(uint32_t events)
{
	setTime(events);
	if (!_canWrite && !events)
		_canWrite = true;
	if (!getIsActive() || !_canWrite)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP))
		_cgiReadFinished = true;
	if (events & EPOLLERR)
	{
		handleCgiError(HTTP_BAD_REQUEST);
		events = 0;
	}
	if (events & EPOLLIN || _cgiReadFinished)
		readFromCgi();
	if (_state == CgiOut::READ_HEADER)
		readHeaders();
	if (_state == CgiOut::CGI_TO_TEMP)
		writeToTemp();
	if (_state == CgiOut::WRITE_FIRST_PART)
		writeFirstPart();
	if (_state == CgiOut::FILE_TO_BUFFER || _state == CgiOut::CGI_TO_BUFFER)
		writeToBuff();
}

void	CgiOut::checkTime(void)
{
	time_t	now = time(NULL);
	if (difftime(now, _lastEpollInTime) > TIMEOUT_VALUE_SEC)
		handleCgiError(HTTP_GATEWAY_TIMEOUT);
}
