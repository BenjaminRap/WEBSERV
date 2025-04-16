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
	std::cout << "TODO  : renvoyer un HTTP_CONTENT_TOO_LARGE quand le body est trop grand" << std::endl;
	std::cout << "TODO  : renvoyer un REQUEST_HEADERS_FIELD_TOO_LARGE quand les headers sont trop grand" << std::endl;
	std::cout << "TODO  : renvoyer un URI_TOO_LONG quand la status line est trop grande" << std::endl;
	std::cout << "TODO  : voir si il faut renvoyer un LENGTH_REQUIRED avec un post" << std::endl;
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
	_state = REQUEST_STATUS_LINE;
	_request.reset();
}

/*****************************Getter / Setter**********************************/

bool		RequestHandler::isStateRequestBody(void)
{
	return (_state == REQUEST_BODY);
}
