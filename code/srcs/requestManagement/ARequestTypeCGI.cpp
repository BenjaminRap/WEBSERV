#include <stdint.h>                 // for uint16_t
#include <sys/types.h>              // for pid_t
#include <cstring>                  // for NULL, memset
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, basic_ios, basic_o...
#include <string>                   // for char_traits, string

#include "ABody.hpp"                // for ABody
#include "AEPollFd.hpp"             // for AEPollFd
#include "ARequestType.hpp"         // for ARequestType
#include "CgiIn.hpp"                // for CgiIn
#include "CgiOut.hpp"               // for CgiOut
#include "CgiOutArgs.hpp"           // for CgiOutArgs
#include "ChunkedBody.hpp"          // for ChunkedBody
#include "EPollHandler.hpp"         // for EPollHandler
#include "Request.hpp"              // for Request
#include "RequestContext.hpp"       // for RequestContext
#include "SharedResource.hpp"       // for SharedResource
#include "SizedBody.hpp"            // for SizedBody
#include "requestStatusCode.hpp"    // for HTTP_OK
#include "socketCommunication.hpp"  // for closeFdAndPrintError

void		deleteArray(const char** array);
void		setArgv(const char* (&argv)[3], const std::string& interpreter, const std::string& cgiFile);
int			execCGI(const char * const argv[3], const char * const env[23], int& inFd, int& outFd);
int			getCGIStatus(pid_t pid);

uint16_t	ARequestType::setCgiAFdData(RequestContext& requestContext)
{
	const char*			env[23];
	const char*			argv[3];
	ABody * const		body = requestContext._request.getBody();
	const CgiOutArgs*	cgiOutArgs = NULL;
	int					inFd = -1;
	int					outFd = -1;
	pid_t				pid = -1;

	std::memset(env, 0, sizeof(env));
	std::memset(argv, 0, sizeof(argv));
	try
	{
		setEnv(env, requestContext);
		setArgv(argv, getCgiInterpreter(), _path);
		cgiOutArgs = new CgiOutArgs(_config, requestContext._responseBuff, getAddHeader());
		if (body != NULL && body->getType() == ABody::CHUNKED)
		{
			setCgiInChunked(requestContext, env, argv, cgiOutArgs, *static_cast<ChunkedBody*>(body));
			return (HTTP_OK);
		}

		pid = execCGI(argv, env, inFd, outFd);
		if (pid == -1)
			throw std::exception();

		if (body != NULL)
			setCgiInSized(requestContext, inFd, *static_cast<SizedBody*>(body));
		else
			closeFdAndPrintError(inFd);
		inFd = -1;
		setCgiOut(requestContext._ePollHandler, outFd, pid, *cgiOutArgs);
		outFd = -1;
		pid = -1;

		delete cgiOutArgs;
		deleteArray((const char**)env);
		deleteArray((const char**)argv);
	}
	catch (const std::exception& e)
	{
		deleteArray((const char**)env);
		deleteArray((const char**)argv);
		delete cgiOutArgs;
		if (pid != -1)
			getCGIStatus(pid);
		closeFdAndPrintError(inFd);
		closeFdAndPrintError(outFd);
		throw ;
	}
	_isCgi = true;
	return (HTTP_OK);
};

void	ARequestType::setCgiInChunked
(
	RequestContext& requestContext,
	const char* (&env)[23],
	const char* (&argv)[3],
	const CgiOutArgs*& cgiOutArgs,
	ChunkedBody& body
)
{

	CgiIn * const	cgiIn = new CgiIn(requestContext._ePollHandler,
		requestContext._requestBuff,
		body,
		requestContext._connectedSocketData,
		requestContext._response,
		argv,
		env,
		cgiOutArgs
	);
	std::cout << "TYPE : " << body.getType() << std::endl;
	env[0] = NULL;
	argv[0] = NULL;
	cgiOutArgs = NULL;
	if (!requestContext._ePollHandler.addFdToList(*cgiIn))
	{
		delete cgiIn ;
		throw std::exception();
	}
	_inFd.setManagedResource(cgiIn, AEPollFd::removeFromEPollHandler);
}

void	ARequestType::setCgiInSized
(
	RequestContext& requestContext,
	int& fd,
	SizedBody& body
)
{
	CgiIn * const	cgiIn = new CgiIn(
		fd,
		requestContext._ePollHandler,
		requestContext._requestBuff,
		body,
		requestContext._connectedSocketData,
		requestContext._response
	);
	fd = -1;
	if (!requestContext._ePollHandler.addFdToList(*cgiIn))
	{
		delete cgiIn;
		throw std::exception();
	}
	_inFd.setManagedResource(cgiIn, AEPollFd::removeFromEPollHandler);
}

void	ARequestType::setCgiOut(
	EPollHandler& ePollHandler,
	int& fd,
	pid_t& pid,
	const CgiOutArgs& cgiOutArgs
)
{
	CgiOut * const	cgiOut = new CgiOut(
		fd,
		ePollHandler,
		pid,
		cgiOutArgs
	);
	fd = -1;
	pid = -1;
	if (!ePollHandler.addFdToList(*cgiOut))
	{
		delete cgiOut;
		throw std::exception();
	}
	_outFd.setManagedResource(cgiOut, AEPollFd::removeFromEPollHandler);
}
