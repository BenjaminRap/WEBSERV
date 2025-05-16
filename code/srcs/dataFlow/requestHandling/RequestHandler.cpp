#include <exception>              // for exception
#include <iostream>               // for basic_ostream, char_traits, operator<<
#include <vector>                 // for vector
#include <sstream>				  // for stringstream

#include "Request.hpp"            // for operator<<, Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState, REQUE...
#include "Response.hpp"           // for Response
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR

class EPollHandler;
class ServerConfiguration;  // lines 10-10

/************************Constructors/Destructors******************************/

RequestHandler::RequestHandler(const std::vector<ServerConfiguration>	&serverConfs) :
	_buffer(),
	_flowBuf(_buffer, REQUEST_BUFFER_SIZE, 0),
	_state(REQUEST_STATUS_LINE),
	_request(),
	_serverConfs(serverConfs)
{
}

RequestHandler::~RequestHandler()
{
}

/************************private Member function*******************************/

RequestState			RequestHandler::readRequest(Response &response, int socketFd, EPollHandler& ePollHandler)
{
	try
	{
		readStatusLine(response);
		readHeaders(response);
		executeRequest(response, ePollHandler);
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

void						RequestHandler::handleCookie(Headers &headers_response, const ServerConfiguration &serverConfiguration)
{
	const std::string	*cookie = this->_request.getHeaders().getHeader("cookie");

	if (cookie == NULL)
	{
		//generating an ID for the user
		static unsigned int	i = 0;
		std::ostringstream	oss;
		oss << i;

		headers_response.insert(std::make_pair("Set-Cookie", "MyBigCookie=" + oss.str() + "; Path=" + this->_request.getRequestTarget() + "; Mage-Age=" + MAX_AGE_COOKIE_SEC + "; Domain=" + serverConfiguration.getShortestServerName()));
		i++;
	}
}
