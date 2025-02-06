#include <iostream>

#include "RequestHandler.hpp"
#include "SizedBody.hpp"

/************************Constructors/Destructors******************************/

RequestHandler::RequestHandler() :
	_buffer(),
	_flowBuffer(_buffer, REQUEST_BUFFER_SIZE, 0),
	_state(REQUEST_STATUS_LINE),
	_request()
{
}

RequestHandler::~RequestHandler()
{
}

/************************private Member function*******************************/

/**
 * the return of parsing functions : 0 : OK, -1 : parsing error, other : errors
 */
int	parseStatusLine(char *line, size_t lineLength)
{
	std::cout << "statusLine : " << std::string(line, lineLength) << ", length : " << lineLength << std::endl;
	return (0);
}
int	parseHeader(char *line, size_t lineLength)
{
	std::cout << "header : " << std::string(line, lineLength) << ", length : " << lineLength << std::endl;
	return (0);
}
Body	*getBody()
{
	return (NULL);
}

void	setResponseCode(int code)
{
	(void)code;
}

void	RequestHandler::readStatusLine()
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
	const int	parsingReturn = parseStatusLine(line, lineLength);
	if (parsingReturn != 0)
	{
		if (parsingReturn == -1)
			setResponseCode(400);
		else
			setResponseCode(500);
		_state = REQUEST_DONE;
		return ;
	}
	_state = REQUEST_HEADERS;
}

void	RequestHandler::readHeaders()
{
	char	*line;
	size_t	lineLength;

	if (_state != REQUEST_HEADERS)
		return ;
	while (_flowBuffer.getLine(&line, &lineLength))
	{
		if (lineLength == 0)
		{
			std::cout << "empty line !" << std::endl;
			_state = REQUEST_EMPTY_LINE;
			return ;
		}
		const int	parsingReturn = parseHeader(line, lineLength);
		if (parsingReturn != 0)
		{
			if (parsingReturn == -1)
				setResponseCode(400);
			else
				setResponseCode(500);
			_state = REQUEST_DONE;
			return ;
		}
	}
}

void	RequestHandler::executeRequest()
{
	if (_state != REQUEST_EMPTY_LINE)
		return ;
}

void	RequestHandler::writeBodyFromBuffer()
{
	if (_state != REQUEST_BODY)
		return ;
	Body * const	body = getBody();

	if (body == NULL)
	{
		_state = REQUEST_DONE;
		return ;
	}
	const FlowState flowState = body->writeBodyFromBufferToFile(_flowBuffer);
	
	if (flowState == FLOW_ERROR)
	{
		setResponseCode(500);
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_DONE && body->getFinished())
		_state = REQUEST_DONE;
}

/*************************public Member function*******************************/

RequestState			RequestHandler::redirectBodySocketToFile(int socketFd)
{
	Body * const	body = getBody();
	
	if (body == NULL)
		return (REQUEST_DONE);
	const FlowState flowState = body->redirectBodyFromSocketToFile(_flowBuffer, socketFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		setResponseCode(500);
		_state = REQUEST_DONE;
	}
	else if (body->getFinished())
		_state = REQUEST_DONE;
	return (_state);
}


RequestState	RequestHandler::redirectSocketToBuffer(int socketFd)
{
	FdType			socketType = SOCKETFD;
	const FlowState flowState = _flowBuffer.redirectFdContentToBuffer(socketFd, socketType);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		setResponseCode(500);
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_BUFFER_FULL)
	{
		setResponseCode(400);
		_state = REQUEST_DONE;
	}
	return (_state);
}

RequestState			RequestHandler::readRequest()
{
	readStatusLine();
	readHeaders();
	executeRequest();
	writeBodyFromBuffer();
	return (_state);
}

/*****************************Getter / Setter**********************************/

bool		RequestHandler::isRequestBody(void)
{
	return (_state == REQUEST_BODY);
}
