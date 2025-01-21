#include "ResponsesHandler.hpp"

ResponsesHandler::ResponsesHandler() :
	_responses(),
	_buffer(),
	_responseBuffer(_buffer, RESPONSE_BUFFER_SIZE)
{

}

ResponsesHandler::~ResponsesHandler()
{

}