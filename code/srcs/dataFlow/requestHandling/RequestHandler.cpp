#include <exception>              // for exception
#include <iostream>               // for basic_ostream, char_traits, operator<<
#include <vector>                 // for vector

#include "Request.hpp"            // for operator<<, Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState, REQUE...
#include "RequestContext.hpp"
#include "Response.hpp"           // for Response
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR

class EPollHandler;
class ServerConfiguration;  // lines 10-10

/************************Constructors/Destructors******************************/

RequestHandler::RequestHandler(const std::vector<ServerConfiguration>	&serverConfs) :
	_buffer(),
	_requestBuf(_buffer, REQUEST_BUFFER_SIZE, 0),
	_state(REQUEST_STATUS_LINE),
	_request(),
	_serverConfs(serverConfs)
{
}

RequestHandler::~RequestHandler()
{
}

/************************private Member function*******************************/

RequestState			RequestHandler::readRequest(int socketFd, RequestContext& requestContext)
{
	Response&	response = requestContext.response;

	try
	{
		readStatusLine(response);
		readHeaders(response);
		executeRequest(response, requestContext);
		redirectBody(socketFd, response, false);
	}
	catch (const std::exception& exception)
	{
		response.setResponse(HTTP_INTERNAL_SERVER_ERROR);
		std::cerr << exception.what() << std::endl;
	}
	return (_state);
}


void			RequestHandler::setNewRequest()
{
	std::cout << "request : " << _request << std::endl;
	_state = REQUEST_STATUS_LINE;
	_request.reset();
}

/*****************************Getter / Setter**********************************/

bool		RequestHandler::isStateRequestBody(void)
{
	return (_state == REQUEST_BODY);
}

Request&	RequestHandler::getRequest(void)
{
	return (_request);
}

FlowBuffer&	RequestHandler::getFlowBuffer(void)
{
	return (_requestBuf);
}
