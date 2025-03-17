#include "RequestHandler.hpp"

void	RequestHandler::readStatusLine(Response &response)
{
	char	*line;
	size_t	lineLength;

	if (_state != REQUEST_STATUS_LINE)
		return ;
	// skip empty lines
	do
	{
		if (!_flowBuffer.getLine(&line, &lineLength))
			return ;
	}
	while (lineLength == 0);
	const int	statusCode = _request.parseStatusLine(line, lineLength);
	if (statusCode != 0)
	{
		response.setResponse(statusCode, "");
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
		if (lineLength == 0)
		{
			_state = REQUEST_EMPTY_LINE;
			return ;
		}
		const int	statusCode = _request.parseHeader(line, lineLength);
		if (statusCode != 0)
		{
			response.setResponse(statusCode, "");
			_state = REQUEST_DONE;
			return ;
		}
	}
}


