#include <stddef.h>                 // for NULL
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN, EPOL...
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...

#include "AEPollFd.hpp"             // for AEPollFd
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData, CONNECTED_EV...
#include "FlowBuffer.hpp"           // for FlowState
#include "Host.hpp"                 // for sockaddr_in_u, Host (ptr only)
#include "RequestHandler.hpp"       // for RequestHandler, RequestState
#include "Response.hpp"             // for Response
#include "ResponsesHandler.hpp"     // for ResponsesHandler
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "Status.hpp"               // for Status, StatusType
#include "exception.hpp"            // for ExecveException
#include "requestStatusCode.hpp"	// for HTTP codes
#include "protocol.hpp"				// for TIMEOUT

class EPollHandler;  // lines 20-20

/*************************Constructors / Destructors***************************/

ConnectedSocketData::ConnectedSocketData
(
	int fd,
	EPollHandler &ePollHandler,
	const std::vector<ServerConfiguration> &serverConfiguration,
	const Host& host,
	const sockaddr_in_u clientAddr
) :
	AEPollFd
	(
		fd,
		ePollHandler,
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
	),
	_serverConfigurations(serverConfiguration)
{

}

ConnectedSocketData::~ConnectedSocketData(void)
{
}

/***************************Member functions***********************************/

RequestState	ConnectedSocketData::processRequest(void)
{
	RequestState	requestState;
	Response		&currentResponse = _responsesHandler.getCurrentResponse();
	const int		fd = getFd();

	if (_requestHandler.isStateRequestBody())
	{
		requestState = _requestHandler.redirectBody(&fd, currentResponse);
	}
	else
	{
		requestState = _requestHandler.redirectFirstPart(fd, currentResponse);
		
		if (requestState != REQUEST_DONE)
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
	setTime(events);
	if (!getIsActive())
		return ;
	bool	shouldRemoveFromEPoll = false;

	try
	{
		if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
			shouldRemoveFromEPoll = true;
		else
		{
			if (!_closing && events & EPOLLIN)
			{
				processRequest();
			}
			if (events & EPOLLOUT)
			{
				const FlowState	flowState = _responsesHandler.sendResponseToSocket(getFd());
				if (flowState == FLOW_ERROR || (_closing && flowState == FLOW_DONE))
					shouldRemoveFromEPoll = true;
			}
		}
	}
	catch(const ExecveException& e)
	{
		throw;
	}
	catch (const std::exception& exception)
	{
		std::cerr << "can't print response, removing the connected fd !\n";
		std::cerr << exception.what() << std::endl;
		shouldRemoveFromEPoll = true;
	}
	if (shouldRemoveFromEPoll == true)
		removeFromEPollHandler();
}

void			ConnectedSocketData::checkTime(time_t now)
{
	if (_closing || !getIsActive())
		return ;

	if (difftime(now, _lastEpollOutTime) > TIMEOUT)
		removeFromEPollHandler();
	else if (difftime(now, _lastEpollInTime) > TIMEOUT)
		_closing = true;
}
