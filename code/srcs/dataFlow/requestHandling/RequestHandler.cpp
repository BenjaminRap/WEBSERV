#include <stddef.h>               // for NULL
#include <exception>              // for exception
#include <iostream>               // for char_traits, basic_ostream, operator<<
#include <vector>                 // for vector

#include "Request.hpp"            // for Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState, REQUE...
#include "Response.hpp"           // for Response
#include "exception.hpp"          // for ExecveException
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR

class FlowBuffer;  // lines 13-13
class RequestContext;
class ServerConfiguration;  // lines 14-14

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

RequestState			RequestHandler::readRequest(RequestContext& requestContext, Response& response)
{
	try
	{
		readStatusLine(response);
		readHeaders(response);
		executeRequest(response, requestContext);
		redirectBody(NULL, response);
	}
	catch (const ExecveException& e)
	{
		throw;
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
