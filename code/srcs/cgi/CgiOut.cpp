#include <stdint.h>                 // for uint16_t, uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOL...
#include <sys/types.h>              // for pid_t, time_t
#include <time.h>                   // for difftime
#include <cstdio>                   // for remove, NULL
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...

#include "AEPollFd.hpp"             // for AEPollFd
#include "AFdData.hpp"              // for AFdData
#include "CgiOut.hpp"               // for CgiOut, CGI_OUT_EVENTS
#include "CgiOutArgs.hpp"           // for CgiOutArgs
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "protocol.hpp"             // for TIMEOUT
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_GATEWAY_TI...

class EPollHandler;  // lines 14-14

int	getCGIStatus(pid_t pid);

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	pid_t pid,
	const CgiOutArgs& cgiOutArgs,
	ConnectedSocketData& connectedSocketData
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
	_addHeader(cgiOutArgs._addHeader),
	_connectedSocketData(connectedSocketData)
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
	try
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
	catch (const std::exception& exception)
	{
		std::cerr << "can't print response !\n";
		std::cerr << exception.what() << std::endl;
		AEPollFd::removeFromEPollHandler(&_connectedSocketData);
		setFinished();
	}
}

void	CgiOut::checkTime(time_t now)
{
	if (!getIsActive())
		return ;

	if (difftime(now, _lastEpollInTime) > TIMEOUT)
		handleCgiError(HTTP_GATEWAY_TIMEOUT);
}
