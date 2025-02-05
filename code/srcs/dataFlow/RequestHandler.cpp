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

/************************Member function***************************************/

bool	parseStatusLine(char *line, size_t lineLength)
{
	std::cout << "statusLine : " << std::string(line, lineLength) << ", length : " << lineLength << std::endl;
	return (true);
}
bool	parseHeader(char *line, size_t lineLength)
{
	std::cout << "header : " << std::string(line, lineLength) << ", length : " << lineLength << std::endl;
	return (true);
}
Body	*getBody()
{
	return (NULL);
}

int	RequestHandler::readStatusLine()
{
	char	*line;
	size_t	lineLength;

	if (_state != REQUEST_STATUS_LINE)
		return (0);
	// skip empty lines
	do
	{
		if (!_flowBuffer.getLine(&line, &lineLength))
			return (1);
	}
	while (lineLength == 0);
	if (!parseStatusLine(line, lineLength))
		return (-1);
	_state = REQUEST_HEADERS;
	return (0);
}

int	RequestHandler::readHeaders()
{
	char	*line;
	size_t	lineLength;

	if (_state != REQUEST_HEADERS)
		return (0);
	while (_flowBuffer.getLine(&line, &lineLength))
	{
		if (lineLength == 0)
		{
			std::cout << "empty line !" << std::endl;
			_state = REQUEST_EMPTY_LINE;
			return (0);
		}
		if (!parseHeader(line, lineLength))
			return (-1);
	}
	return (1);
}

int	RequestHandler::executeRequest()
{
	return (0);
}

int	RequestHandler::readBodyFromBuffer()
{
	if (_state != REQUEST_BODY)
		return (0);
	Body * const	body = getBody();

	if (body == NULL)
		return (FLOW_MORE);
	return (body->writeBodyFromBufferToFile(_flowBuffer));
}

FlowState			RequestHandler::redirectBody(int socketFd)
{
	Body * const	body = getBody();
	
	if (body == NULL)
		return (FLOW_MORE);
	return (body->redirectBodyFromSocketToFile(_flowBuffer, socketFd));
}

FlowState			RequestHandler::readRequest(int socketFd)
{
	FdType			socketType = SOCKETFD;
	const FlowState flowState = _flowBuffer.redirectFdContentToBuffer(socketFd, socketType);

	if (flowState != FLOW_MORE)
		return (flowState);
	int	ret;

	if ((ret = readStatusLine()) != 0)
		return ((FlowState)ret);
	if ((ret = readHeaders()) != 0)
		return ((FlowState)ret);
	if ((ret = executeRequest()) != 0)
		return ((FlowState)ret);
	ret = readBodyFromBuffer();
	return (FLOW_MORE);
}

FlowState	RequestHandler::processRequest(int socketFd)
{
	if (_state == REQUEST_BODY)
	{
		const FlowState flowState = redirectBody(socketFd);
		if (flowState != FLOW_MORE)
			return (flowState);
	}
	else
	{
		readRequest(socketFd);
	}
	const Body * const body = getBody();
	if (_state == REQUEST_BODY && body->getFinished())
		_state = REQUEST_STATUS_LINE;
	return (FLOW_MORE);
}