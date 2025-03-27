#include <exception>				// for std::exception
#include <vector>              		// for vector

#include "Request.hpp"         		// for Request
#include "requestStatusCode.hpp"	// for HTTP_INTERNAL_SERVER_ERROR
#include "RequestHandler.hpp"  		// for RequestHandler, RequestState, REQUEST_...

class Response;
class ServerConfiguration;

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
	_state = REQUEST_STATUS_LINE;
	_request.reset();
}

/*****************************Getter / Setter**********************************/

bool		RequestHandler::isRequestBody(void)
{
	return (_state == REQUEST_BODY);
}
