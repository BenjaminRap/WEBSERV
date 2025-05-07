#include "FlowBuffer.hpp"         // for FlowBuffer
#include "Headers.hpp"            // for Headers
#include "Request.hpp"            // for Request
#include "RequestHandler.hpp"     // for RequestHandler, RequestState
#include "Response.hpp"           // for Response
#include "requestStatusCode.hpp"  // for HTTP_OK

void	RequestHandler::readStatusLine(Response &response)
{
	char	*lineBegin;
	char	*lineEnd;

	if (_state != REQUEST_STATUS_LINE)
		return ;
	if (!_flowBuf.getLine(&lineBegin, &lineEnd))
		return ;
	const int	statusCode = _request.parseStatusLine(lineBegin, lineEnd);
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
	char		*lineBegin;
	char		*lineEnd;
	Headers&	headers = _request.getHeaders();

	if (_state != REQUEST_HEADERS)
		return ;
	while (_flowBuf.getLine(&lineBegin, &lineEnd))
	{
		if (lineBegin == lineEnd - 1 && *lineBegin == '\r')
		{
			_state = REQUEST_EMPTY_LINE;
			return ;
		}
		const int	statusCode = headers.parseHeader(lineBegin, lineEnd);
		if (statusCode != HTTP_OK)
		{
			response.setResponse(statusCode);
			_state = REQUEST_DONE;
			return ;
		}
	}
}


