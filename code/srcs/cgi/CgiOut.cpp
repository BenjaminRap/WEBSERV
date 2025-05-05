#include <cstdlib>					// for std::strtoul
#include <sys/epoll.h>				// for EPOLLIN

#include "CgiOut.hpp"				// for CgiOut
#include "FlowBuffer.hpp"			// for FlowBUffer
#include "requestStatusCode.hpp"	// for HTTP_...

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
	_serverConf(serverConfiguration)
{
	_tempName[0] = '\0';
}

CgiOut::~CgiOut()
{
	if (_srcFile != NULL)
	{
		if (_tempName[0] != '\0')
			std::remove(_tempName);
		delete _srcFile;
	}
}

void	CgiOut::setFinished(void)
{
	_isActive = false;
	_state = DONE;
}

std::string	sizeTToString(size_t value);

void	CgiOut::handleClosingCgi()
{
	if (_state == READ_HEADER)
	{
		_code = HTTP_BAD_GATEWAY;
		_error = true;
		generateFirstPart();
	}
	else if (_state == CGI_TO_TEMP)
	{
		delete _srcFile;
		_srcFile = FileFd::getTemporaryFile(_tempName, O_RDONLY);
		if (_srcFile == NULL)
		{
			_code = HTTP_INTERNAL_SERVER_ERROR;
			_error = true;
		}
		else
			_headers["content-length"] = sizeTToString(_srcFile->getSize());
		generateFirstPart();
	}
	else
		setFinished();
}

void	CgiOut::callback(uint32_t events)
{
	if (!_isActive || _state == DONE)
		return ;
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
		handleClosingCgi();
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
