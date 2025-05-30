#include <netinet/in.h>             // for sockaddr_in
#include <stddef.h>                 // for NULL
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOL...
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...
#include <vector>                   // for vector

#include "AFdData.hpp"              // for AFdDataChilds
#include "ASocketData.hpp"          // for ASocketData
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData, CONNECTED_EV...
#include "FlowBuffer.hpp"           // for FlowState
#include "RequestHandler.hpp"       // for RequestHandler, RequestState
#include "Response.hpp"             // for Response
#include "ResponsesHandler.hpp"     // for ResponsesHandler
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "Status.hpp"               // for Status, StatusType
#include "exception.hpp"            // for ExecveException

class EPollHandler;  // lines 19-19
class Host;

/*************************Constructors / Destructors***************************/

ConnectedSocketData::ConnectedSocketData
(
	int fd,
	EPollHandler &ePollHandler,
	const std::vector<ServerConfiguration> &serverConfiguration,
	const Host& host,
	const sockaddr_in_u clientAddr
) :
	ASocketData
	(
		fd,
		ePollHandler,
		serverConfiguration,
		CONNECTED_SOCKET_DATA,
		CONNECTED_EVENTS
	),
	_responsesHandler(serverConfiguration.front()),
	_requestHandler(serverConfiguration),
	_closing(false),
	_requestContext
	(
		host,
		clientAddr,
		_requestHandler.getRequest(),
		_responsesHandler.getCurrentResponse(),
		ePollHandler,
		_requestHandler.getFlowBuffer(),
		_responsesHandler.getFlowBuffer(),
		*this
	)
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
		requestState = _requestHandler.redirectBody(&_fd, currentResponse);
	}
	else
	{
		requestState = _requestHandler.redirectFirstPart(_fd, currentResponse);
		
		if (requestState != CONNECTION_CLOSED && requestState != REQUEST_DONE)
			requestState = _requestHandler.readRequest(_requestContext, currentResponse);
	}
	requestState = readNextRequests(currentResponse, requestState);
	return (requestState);
}


RequestState	ConnectedSocketData::readNextRequests
(
	Response &currentResponse,
	RequestState requestState
)
{
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
		requestState = _requestHandler.readRequest(_requestContext, currentResponse);
	}
	return (requestState);
}

void	ConnectedSocketData::ignoreBodyAndReadRequests(Response& response)
{
	const RequestState state = _requestHandler.ignoreBody(response);

	readNextRequests(response, state);
}

void	ConnectedSocketData::callback(uint32_t events)
{
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
		_isActive = false;
	try
	{
		if (_isActive && !_closing && events & EPOLLIN)
		{
			if (processRequest() == CONNECTION_CLOSED)
				_isActive = false;
		}
		if (_isActive && events & EPOLLOUT)
		{
			const FlowState	flowState = _responsesHandler.sendResponseToSocket(_fd);
			if (flowState == FLOW_ERROR || (_closing && flowState == FLOW_DONE))
				_isActive = false;
		}
	}
	catch(const ExecveException& e)
	{
		throw;
	}
	catch (const std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		_isActive = false;
	}
	if (_isActive == false)
		removeFromEPollHandler();
}
