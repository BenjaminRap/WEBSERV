#include <iostream>

#include "RequestHandler.hpp"
#include "SizedBody.hpp"

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

int	RequestHandler::readBody()
{
	if (_state != REQUEST_BODY)
		return (0);
	Body * const body = getBody();

	if (body == NULL)
		return (0);
	SizedBody * const sizedBody = dynamic_cast<SizedBody *>(body);
	if (sizedBody != NULL)
	{
		const FlowState flowState = _flowBuffer.redirectBufferContentToFd(sizedBody->getFd(), \
			 *sizedBody, SizedBody::writeToFile);
		return (flowState);
	}
	std::cerr << "Logic Error : readBody() : the body type is unkown" << std::endl;
	return (-1);
}


FlowState	RequestHandler::processRequest(int socketFd)
{
	FdType	socketType = SOCKETFD;

	if (_state == REQUEST_BODY)
	{
		
	}
	else
	{
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
		if ((ret = readBody()) != 0)
			return ((FlowState)ret);
	}
	const Body * const body = getBody();
	if (_state == REQUEST_BODY && (body == NULL || body->getFinished()))
	{
		_state = REQUEST_STATUS_LINE;
	}
	return (FLOW_MORE);
}