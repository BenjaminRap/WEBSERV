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

void	RequestHandler::readStatusLine(Response &response)
{
	char	*lineStart;
	char	*lineLast;

	if (_state != REQUEST_STATUS_LINE)
		return ;
	// skip empty lines
	do
	{
		if (!_flowBuffer.getLine(&lineStart, &lineLast))
			return ;
	}
	while (lineStart + 1 == lineLast);
	const int	parsingReturn = _request.parseStatusLine(lineStart, lineLast);
	if (parsingReturn != 0)
	{
		if (parsingReturn == -1)
			response.setResponseCode(400, "Bad Request");
		else
			response.setResponseCode(500, "Internal Server Error");
		_state = REQUEST_DONE;
		return ;
	}
	_state = REQUEST_HEADERS;
}

void	RequestHandler::readHeaders(Response &response)
{
	char	*lineStart;
	char	*lineLast;

	if (_state != REQUEST_HEADERS)
		return ;
	while (_flowBuffer.getLine(&lineStart, &lineLast))
	{
		if (lineStart + 1 == lineLast)
		{
			std::cout << "empty line !" << std::endl;
			_state = REQUEST_EMPTY_LINE;
			return ;
		}
		const int	parsingReturn = _request.parseHeader(lineStart, lineLast);
		if (parsingReturn != 0)
		{
			if (parsingReturn == -1)
				response.setResponseCode(400, "Bad Request");
			else
				response.setResponseCode(500, "Internal Server Error");
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
		response.setResponseCode(500, "Internal Server Error");
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
		response.setResponseCode(500, "Internal Server Error");
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
		response.setResponseCode(500, "Internal Server Error");
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_BUFFER_FULL)
	{
		response.setResponseCode(400, "Bad Request");
		_state = REQUEST_DONE;
	}
	return (_state);
}

RequestState	RequestHandler::readRequest(Response &response)
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