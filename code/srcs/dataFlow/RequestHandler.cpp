#include <iostream>

#include "RequestHandler.hpp"

/************************Constructors/Destructors******************************/

RequestHandler::RequestHandler(const std::vector<ServerConfiguration>	&serverConfs) :
	_buffer(),
	_flowBuffer(_buffer, REQUEST_BUFFER_SIZE, 0),
	_state(REQUEST_STATUS_LINE),
	_request(),
	_serverConfs(serverConfs)
{
}

RequestHandler::~RequestHandler()
{
}

/************************private Member function*******************************/

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
		if (lineLength == 0)
		{
			_state = REQUEST_EMPTY_LINE;
			return ;
		}
		const int	statusCode = _request.parseHeader(line, lineLength);
		if (statusCode != 0)
		{
			response.setResponse(statusCode);
			_state = REQUEST_DONE;
			return ;
		}
	}
}

void	RequestHandler::executeRequest(Response &response)
{
	if (_state != REQUEST_EMPTY_LINE)
		return ;

	_state = REQUEST_DONE;
	(void)response;
}

void	RequestHandler::writeBodyFromBuffer(Response &response)
{
	if (_state != REQUEST_BODY)
		return ;
	Body * const	body = _request.getBody();

	if (body == NULL)
	{
		_state = REQUEST_DONE;
		return ;
	}
	const FlowState flowState = body->writeBodyFromBufferToFile(_flowBuffer);
	
	if (flowState == FLOW_ERROR)
	{
		response.setResponse(500);
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_DONE && body->getFinished())
		_state = REQUEST_DONE;
}

/*************************public Member function*******************************/

RequestState			RequestHandler::redirectBodySocketToFile(int socketFd, Response &response)
{
	Body * const	body = _request.getBody();
	
	if (body == NULL)
		return (REQUEST_DONE);
	const FlowState flowState = body->redirectBodyFromSocketToFile(_flowBuffer, socketFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(500);
		_state = REQUEST_DONE;
	}
	else if (body->getFinished())
		_state = REQUEST_DONE;
	return (_state);
}

RequestState	RequestHandler::redirectSocketToBuffer(int socketFd, Response &response)
{
	FdType			socketType = SOCKETFD;
	const FlowState flowState = _flowBuffer.redirectFdContentToBuffer(socketFd, socketType);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(500);
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_BUFFER_FULL)
	{
		response.setResponse(400);
		_state = REQUEST_DONE;
	}
	return (_state);
}

RequestState			RequestHandler::readRequest(Response &response)
{
	readStatusLine(response);
	readHeaders(response);
	executeRequest(response);
	writeBodyFromBuffer(response);
	return (_state);
}


void			RequestHandler::setNewRequest()
{
	_state = REQUEST_STATUS_LINE;
	_request.reset();
}

/*****************************Getter / Setter**********************************/

bool		RequestHandler::isRequestBody(void)
{
	return (_state == REQUEST_BODY);
}
