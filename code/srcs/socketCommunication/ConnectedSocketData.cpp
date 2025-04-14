#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLIN, EPOLLOUT
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...
#include <vector>                   // for vector

#include "AFdData.hpp"              // for AFdData
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "FlowBuffer.hpp"           // for FlowState
#include "RequestHandler.hpp"       // for RequestHandler, RequestState
#include "ResponsesHandler.hpp"     // for ResponsesHandler
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SocketsHandler.hpp"       // for SocketsHandler
#include "Status.hpp"				// for Status, STATUS_ERROR

class Response;  // lines 11-11

/*************************Constructors / Destructors***************************/

ConnectedSocketData::ConnectedSocketData(int fd, SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> &serverConfiguration) :
	AFdData(fd, socketsHandler, serverConfiguration),
	_responsesHandler(serverConfiguration.front()),
	_requestHandler(serverConfiguration),
	_closing(false)
{

}

ConnectedSocketData::~ConnectedSocketData(void)
{
	std::cout << "ConnectedSocketData removed" << std::endl;
}

/***************************Member functions***********************************/

RequestState	ConnectedSocketData::processRequest(void)
{
	RequestState	requestState;
	Response		&currentResponse = _responsesHandler.getCurrentResponse();

	if (_requestHandler.isStateRequestBody())
	{
		requestState = _requestHandler.redirectBody(_fd, currentResponse);
	}
	else
	{
		requestState = _requestHandler.redirectFirstPart(_fd, currentResponse);
		
		if (requestState != CONNECTION_CLOSED && requestState != REQUEST_DONE)
			requestState = _requestHandler.readRequest(currentResponse, _fd);
	}
	while  (requestState == REQUEST_DONE)
	{
		const Status*	status = currentResponse.getStatus();
		_requestHandler.setNewRequest();
		_responsesHandler.addCurrentResponseToQueue();
		if (status == NULL || status->isOfType(STATUS_ERROR))
		{
			_closing = true;
			return (requestState);
		}
		requestState = _requestHandler.readRequest(currentResponse, _fd);
	}
	return (requestState);
}

void	ConnectedSocketData::callback(uint32_t events)
{
	bool		removeFromListeners = false;

	try
	{
		if (_closing == false && events & EPOLLIN)
		{
			if (processRequest() == CONNECTION_CLOSED)
				removeFromListeners = true;
		}
		if (removeFromListeners == false && events & EPOLLOUT)
		{
			const FlowState	flowState = _responsesHandler.sendResponseToSocket(_fd);
			if (flowState == FLOW_ERROR || (_closing && flowState == FLOW_DONE))
				removeFromListeners = true;
		}
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		removeFromListeners = true;
	}
	if (removeFromListeners)
		_socketsHandler.removeFdDataFromList(_iterator);
}
