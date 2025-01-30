#include <cstring>

#include "RequestHandler.hpp"

/************************Constructors/Destructors******************************/

RequestHandler::RequestHandler() :
	_buffer(),
	_flowBuffer(_buffer, REQUEST_BUFFER_SIZE, 0),
	_state(REQUEST_STATUS_LINE)
{
}

/**
 * @note Does a deep copy of the reference buffer, but only of the buffer length.
 * Ex: if the ref flowBuffer has a bufferLength of 3, only 3 characters will be
 * copied.
 */
RequestHandler::RequestHandler(const RequestHandler& ref) :
	_flowBuffer(_buffer, REQUEST_BUFFER_SIZE, ref._flowBuffer.getBufferLength()),
	_state(ref._state)
{
	std::memcpy(_buffer, ref._buffer, ref._flowBuffer.getBufferLength());
}

RequestHandler::~RequestHandler()
{
}

/************************Member function***************************************/

bool	RequestHandler::readRequest(int socketFd)
{
}

bool	RequestHandler::processRequest(int socketFd)
{

}