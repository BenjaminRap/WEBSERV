#include <fcntl.h>
#include <stdint.h>                 // for uint16_t, uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN

#include "ABody.hpp"                // for ABody
#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "CgiIn.hpp"                // for CgiIn, CGI_IN_EVENTS
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "FlowBuffer.hpp"           // for FlowState, FlowBuffer
#include "Response.hpp"             // for Response
#include "requestStatusCode.hpp"
#include "CgiOut.hpp"
#include "ChunkedBody.hpp"
#include "SizedBody.hpp"
#include "socketCommunication.hpp"
#include "EPollHandler.hpp"

class EPollHandler;  // lines 14-14

bool	addContentLengthToEnv(const char *(&env)[23], size_t contentLength);
int		execCGI(const char * const argv[3], const char * const env[23], int& inFd, int& outFd);
int		getCGIStatus(pid_t pid);
void	deleteArray(const char** array);

CgiIn::CgiIn
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
	AEPollFd(ePollHandler, CGI_IN),
	_flowBuf(requestFlowBuffer),
	_body(chunkedBody),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse),
	_state(CgiIn::BUFFER_TO_TEMP),
	_tempName(),
	_tmpFile(NULL),
	_argv(),
	_env(),
	_cgiOutArgs(cgiOutArgs)
{
	std::memcpy(_argv, argv, sizeof(_argv));
	std::memcpy(_env, env, sizeof(_env));
	_tempName[0] = '\0';
	_tmpFile = FileFd::getTemporaryFile(_tempName);
	if (_tmpFile == NULL)
		throw std::runtime_error("Can't open a temporary file !");
}

CgiIn::CgiIn
(
	int fd,
	EPollHandler& ePollHandler,
	FlowBuffer& requestFlowBuffer,
	SizedBody& sizedBody,
	ConnectedSocketData& connectedSocketData,
	Response& currentResponse
) :
	AEPollFd(fd, ePollHandler, CGI_IN, CGI_IN_EVENTS),
	_flowBuf(requestFlowBuffer),
	_body(sizedBody),
	_connectedSocketData(connectedSocketData),
	_response(currentResponse),
	_state(CgiIn::BUFFER_TO_CGI),
	_tmpFile(NULL),
	_argv(),
	_env(),
	_cgiOutArgs(NULL)
{
	std::memset(_argv, 0, sizeof(_argv));
	std::memset(_env, 0, sizeof(_env));
	_tempName[0] = '\0';
}

CgiIn::~CgiIn()
{
	if (_tempName[0] != '\0')
		std::remove(_tempName);
	delete _tmpFile;
	delete _cgiOutArgs;
	deleteArray((const char**)_env);
	deleteArray((const char**)_argv);
}

void	CgiIn::execCgi(void)
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
		_body.setFd(inFd);
		inFd = -1;
		CgiOut * const	cgiOut = new CgiOut(
			outFd,
			_ePollHandler,
			pid,
			*_cgiOutArgs
		);
		if (!_ePollHandler.addFdToList(*cgiOut))
		{
			delete cgiOut;
			throw std::exception();
		}
		_response.setFdData(*cgiOut, AEPollFd::removeFromEPollHandler);
		_state = CgiIn::TEMP_TO_CGI;
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

void	CgiIn::setFinished(uint16_t code)
{
	_isActive = false;
	_state = CgiIn::DONE;
	if (code != 0)
		_response.setResponse(code);
	_connectedSocketData.ignoreBodyAndReadRequests(_response);
}

uint16_t	getCodeIfFinished(bool canWrite, FlowState flowResult, const ABody& body);

void	CgiIn::callback(uint32_t events)
{
	if (!_isActive || _state == CgiIn::DONE)
		return ;
	if (events & (EPOLLERR | EPOLLHUP))
	{
		setFinished(0);
		return ;
	}
	if (_state == CgiIn::BUFFER_TO_TEMP)
	{
		const FlowState	flowState = _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);
		const uint16_t	code = getCodeIfFinished(true, flowState, _body);

		if (code == HTTP_OK)
			execCgi();
		else if (code != 0)
			setFinished(code);
		return ;
	}
	if (!(events & EPOLLOUT))
		return ;
	if (_state == CgiIn::BUFFER_TO_CGI)
	{
		const FlowState	flowState = _flowBuf.buffToDest<ABody&>(_body, ABody::writeToFd);
		const uint16_t	code = getCodeIfFinished(true, flowState, _body);

		if (code != 0)
			setFinished(code);
	}
	else if (_state == CgiIn::TEMP_TO_CGI)
	{
		const FlowState	flowState = _flowBuf.redirect(_tmpFile->getFd(), getFd());

		if (flowState == FLOW_ERROR)
			setFinished(HTTP_INTERNAL_SERVER_ERROR);
		else if (flowState == FLOW_DONE)
			setFinished(0);
	}
}
