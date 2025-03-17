#include <iostream>

#include "RequestHandler.hpp"
#include "GetRequest.hpp"
#include "PutRequest.hpp"
#include "DeleteRequest.hpp"

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


const ServerConfiguration&	RequestHandler::getServerConfiguration(void) const
{
	if (_serverConfs.size() == 0)
		throw std::logic_error("The ServerConfiguration vector is empty !");
	const std::string * const host = _request.getHeader("Host");
	if (host == NULL)
		return (_serverConfs[0]);
	for (std::vector<ServerConfiguration>::const_iterator serverIt = _serverConfs.begin(); serverIt != _serverConfs.end(); serverIt++)
	{
		const std::vector<std::string>	serverNames = serverIt->getServerNames();
		if (std::find(serverNames.begin(), serverNames.end(), *host) != serverNames.end())
			return (*serverIt);
	}
	return (_serverConfs[0]);
}

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

void	RequestHandler::executeRequest(Response &response)
{
	if (_state != REQUEST_EMPTY_LINE)
		return ;

	const ServerConfiguration	serverConfiguration = getServerConfiguration();
	std::cout << _request << '\n';
	switch (_request.getMethod())
	{
		case GET: {
			GetRequest	getRequest(_request.getRequestTarget(), serverConfiguration);
			response.setResponse(getRequest.getCode(), getRequest.getRedirection());
			if (getRequest.getCode() >= 200 && getRequest.getCode() < 300)
				_state = REQUEST_BODY;
			else
				_state = REQUEST_DONE;
			std::cout << "GET" << std::endl;
			break;
		}
		case POST: {
			std::cout << "POST" << std::endl;
			break;
		}
		case PUT: {
			PutRequest	putRequest(_request.getRequestTarget(), serverConfiguration);
			response.setResponse(putRequest.getCode(), putRequest.getRedirection());
			_state = REQUEST_DONE;
			std::cout << "PUT" << std::endl;
			break;
		}
		case DELETE: {
			DeleteRequest	deleteRequest(_request.getRequestTarget(), serverConfiguration);
			response.setResponse(deleteRequest.getCode(), deleteRequest.getRedirection());
			_state = REQUEST_DONE;
			std::cout << "DELETE" << std::endl;
			break;
		}
		default:
			throw std::logic_error("executeRequest called with a request method invalid !");
	}
	std::cout << response << std::endl;
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
	const FlowState flowState = _flowBuffer.redirectBufferContentToFd<Body&>(*body, Body::callInstanceWriteToFd);
	
	if (flowState == FLOW_ERROR)
	{
		response.setResponse(500, "");
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
	const FlowState flowState = body->getIsBlocking() ?
		_flowBuffer.redirectFdContentToBuffer<int>(socketFd)
		: _flowBuffer.redirectContent<int, Body&>(socketFd, *body, Body::callInstanceWriteToFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(500, "");
		_state = REQUEST_DONE;
	}
	else if (body->getFinished())
		_state = REQUEST_DONE;
	return (_state);
}

RequestState	RequestHandler::redirectSocketToBuffer(int socketFd, Response &response)
{
	const FlowState flowState = _flowBuffer.redirectFdContentToBuffer<int>(socketFd);

	if (flowState == FLOW_DONE)
		_state = CONNECTION_CLOSED;
	else if (flowState == FLOW_ERROR)
	{
		response.setResponse(500, "");
		_state = REQUEST_DONE;
	}
	else if (flowState == FLOW_BUFFER_FULL)
	{
		response.setResponse(400, "");
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
