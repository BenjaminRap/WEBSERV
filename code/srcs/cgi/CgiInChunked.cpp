#include <fcntl.h>                  // for O_RDONLY
#include <stdint.h>                 // for uint16_t, uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLOUT
#include <sys/types.h>              // for pid_t
#include <cstdio>                   // for NULL, remove, size_t
#include <cstring>                  // for memcpy
#include <exception>                // for exception
#include <stdexcept>                // for runtime_error
#include <string>                   // for basic_string

#include "ABody.hpp"                // for ABody
#include "AEPollFd.hpp"             // for AEPollFd
#include "AFdData.hpp"              // for AFdData
#include "CgiInChunked.hpp"         // for CgiInChunked, CGI_IN_EVENTS, CGI_...
#include "CgiOut.hpp"               // for CgiOut
#include "CgiOutArgs.hpp"           // for CgiOutArgs
#include "ChunkedBody.hpp"          // for ChunkedBody
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "EPollHandler.hpp"         // for EPollHandler
#include "FileFd.hpp"               // for FileFd
#include "FlowBuffer.hpp"           // for FlowState, FlowBuffer
#include "Response.hpp"             // for Response
#include "requestStatusCode.hpp"    // for HTTP_INTERNAL_SERVER_ERROR, HTTP_OK
#include "socketCommunication.hpp"  // for closeFdAndPrintError

bool	addContentLengthToEnv(const char *(&env)[23], size_t contentLength);
int		execCGI(const char * const argv[3], const char * const env[23], int& inFd, int& outFd);
int		getCGIStatus(pid_t pid);
void	deleteArray(const char** array);

CgiInChunked::CgiInChunked
(
	EPollHandler& ePollHandler,
	FlowBuffer& requestFlowBuffer,
	ChunkedBody& chunkedBody,
	ConnectedSocketData& connectedSocketData,
	Response& currentResponse,
	const char* argv[3],
	const char* env[23],
	const CgiOutArgs * cgiOutArgs
) :
	AEPollFd(ePollHandler, CGI_IN_CHUNKED),
	_requestBuf(requestFlowBuffer),
	_body(chunkedBody),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse),
	_state(CgiInChunked::BUFFER_TO_TEMP),
	_tempName(),
	_tmpFile(NULL),
	_argv(),
	_env(),
	_cgiOutArgs(cgiOutArgs),
	_tempBuf(),
	_tempFlowBuf(_tempBuf, CGI_TEMP_BUFFER_CAPACITY, 0)
{
	std::memcpy(_argv, argv, sizeof(_argv));
	std::memcpy(_env, env, sizeof(_env));
	_tempName[0] = '\0';
	_tmpFile = FileFd::getTemporaryFile(_tempName);
	if (_tmpFile == NULL)
		throw std::runtime_error("Can't open a temporary file !");
	_body.setFd(_tmpFile->getFd(), false);
}

CgiInChunked::~CgiInChunked()
{
	if (_tempName[0] != '\0')
		std::remove(_tempName);
	delete _tmpFile;
	delete _cgiOutArgs;
	deleteArray((const char**)_env);
	deleteArray((const char**)_argv);
}

void	CgiInChunked::execCgi(void)
{
	int		inFd = -1;
	int		outFd = -1;
	pid_t	pid = -1;

	try
	{
		delete _tmpFile;
		_tmpFile = NULL;
		_tmpFile = new FileFd(_tempName, O_RDONLY);
		addContentLengthToEnv(_env, _tmpFile->getSize());
		
		pid = execCGI(_argv, _env, inFd, outFd);
		if (pid == -1)
			throw std::exception();

		setFd(inFd, CGI_IN_EVENTS);
		inFd = -1;
		CgiOut * const	cgiOut = new CgiOut(
			outFd,
			_ePollHandler,
			pid,
			*_cgiOutArgs
		);
		outFd = -1;
		pid = -1;
		if (!_ePollHandler.addFdToList(*cgiOut))
		{
			delete cgiOut;
			throw std::exception();
		}
		_response.setFdData(*cgiOut, AEPollFd::removeFromEPollHandler);
		_state = CgiInChunked::TEMP_TO_CGI;
	}
	catch (const std::exception& e)
	{
		closeFdAndPrintError(inFd);
		closeFdAndPrintError(outFd);
		if (pid != -1)
			getCGIStatus(pid);
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
	}
}

void	CgiInChunked::setFinished(uint16_t code)
{
	AFdData::setFinished();
	_state = CgiInChunked::DONE;
	if (code != 0)
		_response.setResponse(code);
	_connectedSocketData.ignoreBodyAndReadRequests(_response);
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body);

void	CgiInChunked::callback(uint32_t events)
{
	setTime(events);
	if (!getIsActive() || _state == CgiInChunked::DONE)
		return ;
	if (events & (EPOLLERR | EPOLLHUP))
	{
		setFinished(0);
		return ;
	}
	if (_state == CgiInChunked::BUFFER_TO_TEMP)
	{
		const FlowState	flowState = _requestBuf.buffToDest<ABody&>(_body, ABody::writeToFd);
		const uint16_t	code = getCodeIfFinished(true, flowState, _body);

		if (code == HTTP_OK)
			execCgi();
		else if (code != 0)
			setFinished(code);
	}
	if (!(events & EPOLLOUT) || _state != CgiInChunked::TEMP_TO_CGI)
		return ;
	const FlowState	flowState = _tempFlowBuf.redirect(_tmpFile->getFd(), getFd());

	if (flowState == FLOW_ERROR)
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
	else if (flowState == FLOW_DONE)
		setFinished(0);
}

void			CgiInChunked::checkTime(void)
{
	time_t	now = time(NULL);
	if (difftime(now, _lastEpollOutTime) > TIMEOUT_VALUE_SEC)
	{
		setFinished(HTTP_REQUEST_TIMEOUT);
	}
}
