#include <fcntl.h>                // for O_RDONLY
#include <stdint.h>               // for uint32_t
#include <sys/epoll.h>            // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOLLR...
#include <cstdio>                 // for NULL, remove, size_t
#include <map>                    // for map
#include <string>                 // for basic_string, string

#include "AFdData.hpp"            // for AFdData, AFdDataChilds
#include "CgiOut.hpp"             // for CgiOut, CgiOutState, CGI_OUT_EVENTS
#include "FileFd.hpp"             // for FileFd
#include "Headers.hpp"            // for Headers
#include "requestStatusCode.hpp"  // for HTTP_BAD_GATEWAY, HTTP_INTERNAL_SER...

class EPollHandler;
class FlowBuffer;
class ServerConfiguration;

CgiOut::CgiOut
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer&	responseFlowBuffer,
	const ServerConfiguration& serverConfiguration
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
	_canWrite(false)
{
	_tempName[0] = '\0';
}

CgiOut::~CgiOut()
{
	if (_srcFile != NULL)
		delete _srcFile;
}

void	CgiOut::setFinished(void)
{
	_isActive = false;
	_state = DONE;
}

void	CgiOut::handleCgiError(void)
{
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
	if (!_isActive || _state == DONE || !_canWrite)
		return ;
	if (events & EPOLLERR)
	{
		events = 0;
		handleCgiError();
	}
	if (events & EPOLLIN)
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
