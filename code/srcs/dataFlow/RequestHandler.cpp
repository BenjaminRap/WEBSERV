#include "RequestHandler.hpp"

/************************Constructors/Destructors******************************/

RequestHandler::RequestHandler() :
	_buffer(),
	_flowBuffer(_buffer, REQUEST_BUFFER_SIZE, 0),
	_state(REQUEST_STATUS_LINE)
{
}

RequestHandler::~RequestHandler()
{
}

/************************Member function***************************************/

bool	RequestHandler::readRequest(int socketFd)
{
	(void)socketFd;
	return (true);
}

bool	RequestHandler::processRequest(int socketFd)
{
	(void)socketFd;
	return (true);
}