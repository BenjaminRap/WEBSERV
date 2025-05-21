#include <stdint.h>               // for uint32_t
#include <sys/epoll.h>            // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOLLR...
#include <sys/types.h>            // for pid_t
#include <cstdio>                 // for remove, NULL

#include "AFdData.hpp"            // for AFdData, AFdDataChilds
#include "CgiOut.hpp"             // for CgiOut, CgiOutState, CGI_OUT_EVENTS
#include "requestStatusCode.hpp"  // for HTTP_BAD_GATEWAY

class EPollHandler;  // lines 15-15
class FlowBuffer;  // lines 16-16
class ServerConfiguration;  // lines 17-17

int	getCGIStatus(pid_t pid);

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer&	responseFlowBuffer,
	const ServerConfiguration& serverConfiguration,
	pid_t pid
) :
	AFdData(fd, ePollHandler, CGI_OUT, CGI_OUT_EVENTS),
	_flowBuf(responseFlowBuffer),
	_firstPart(),
	_charsWritten(0),
	_headers(),
	_srcFile(NULL),
	_state(READ_HEADER),
	_code(0),
	_error(false),
	_serverConf(serverConfiguration),
	_canWrite(false),
	_cgiReadFinished(false),
	_pid(pid)
{
	_tempName[0] = '\0';
}

CgiOut::~CgiOut()
{
	if (_tempName[0] != '\0')
		std::remove(_tempName);
	if (_srcFile != NULL)
		delete _srcFile;
	getCGIStatus(_pid);
}

void	CgiOut::setFinished(void)
{
	_isActive = false;
	_state = DONE;
}

void	CgiOut::handleCgiError(uint32_t& events)
{
	events = 0;
	if (_state == READ_HEADER)
	{
		_code = HTTP_BAD_GATEWAY;
		_error = true;
		generateFirstPart();
	}
	else
		setFinished();
}

bool	CgiOut::isResponseReady(void) const
{
	if (_state == READ_HEADER || _state == CGI_TO_TEMP)
		return (false);
	return (true);
}

void	CgiOut::callback(uint32_t events)
{
	if (!_canWrite && !events)
		_canWrite = true;
	if (!_isActive || !_canWrite)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP))
		_cgiReadFinished = true;
	if (events & EPOLLERR)
		handleCgiError(events);
	if (events & EPOLLIN || _cgiReadFinished)
		readFromCgi();
	if (_state == READ_HEADER)
		readHeaders();
	if (_state == CGI_TO_TEMP)
		writeToTemp();
	if (_state == WRITE_FIRST_PART)
		writeFirstPart();
	if (_state == FILE_TO_BUFFER || _state == CGI_TO_BUFFER)
		writeToBuff();
}
