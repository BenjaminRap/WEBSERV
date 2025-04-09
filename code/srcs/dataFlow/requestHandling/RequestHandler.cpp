#include <exception>              // for exception
#include <iostream>               // for char_traits, basic_ostream, basic_ios
#include <vector>                 // for vector

#include "Request.hpp"            // for Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState, REQUE...
#include "Response.hpp"           // for Response
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR

class ServerConfiguration;  // lines 9-9

/************************Constructors/Destructors******************************/

RequestHandler::RequestHandler(const std::vector<ServerConfiguration>	&serverConfs) :
	_buffer(),
	_flowBuffer(_buffer, REQUEST_BUFFER_SIZE, 0),
	_state(REQUEST_STATUS_LINE),
	_request(),
	_serverConfs(serverConfs)
{
}

RequestHandler::~RequestHandler()
{
}

/************************private Member function*******************************/

RequestState			RequestHandler::readRequest(Response &response, int socketFd)
{
	try
	{
		readStatusLine(response);
		readHeaders(response);
		executeRequest(response, socketFd);
		writeBodyFromBuffer(response);
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
