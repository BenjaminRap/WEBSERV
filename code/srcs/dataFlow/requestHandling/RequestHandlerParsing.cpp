#include <stddef.h>            		// for size_t

#include "FlowBuffer.hpp"			// for FlowBuffer
#include "Request.hpp"				// for Request
#include "RequestHandler.hpp"  		// for RequestHandler, RequestState
#include "Response.hpp"				// for Response
#include "requestStatusCode.hpp"	// for HTTP_...

void	RequestHandler::readStatusLine(Response &response)
{
	char	*line;
	size_t	lineLength;

	if (_state != REQUEST_STATUS_LINE)
		return ;
	if (!_flowBuffer.getLine(&line, &lineLength))
		return ;
	const int	statusCode = _request.parseStatusLine(line, line + lineLength);
	if (statusCode != HTTP_OK)
	{
		response.setResponse(statusCode);
		_state = REQUEST_DONE;
	}
	else
		_state = REQUEST_HEADERS;
}

void	RequestHandler::readHeaders(Response &response)
{
	char	*line;
	size_t	lineLength;

	if (_state != REQUEST_HEADERS)
		return ;
	while (_flowBuffer.getLine(&line, &lineLength))
	{
		if (lineLength == 1 && *line == '\r')
		{
			_state = REQUEST_EMPTY_LINE;
			return ;
		}
		const int	statusCode = _request.parseHeader(line, line + lineLength);
		if (statusCode != HTTP_OK)
		{
			response.setResponse(statusCode);
			_state = REQUEST_DONE;
			return ;
		}
	}
}


