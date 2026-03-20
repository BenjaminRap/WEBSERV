#include <algorithm>                // for find
#include <cstdio>                   // for NULL
#include <stdexcept>                // for logic_error
#include <string>                   // for basic_string, string, operator==
#include <vector>                   // for vector

#include "DeleteRequest.hpp"        // for DeleteRequest
#include "EMethods.hpp"             // for EMethods
#include "GetRequest.hpp"           // for GetRequest
#include "Headers.hpp"              // for Headers
#include "PutRequest.hpp"           // for PutRequest
#include "Request.hpp"              // for Request
#include "RequestHandler.hpp"       // for RequestHandler, RequestState
#include "Response.hpp"             // for Response
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "Status.hpp"               // for Status, StatusType
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_OK
#include "PostRequest.hpp"

class RequestContext;

const ServerConfiguration&	RequestHandler::getServerConfiguration(std::string host) const
{
	if (_serverConfs.size() == 0)
		throw std::logic_error("The ServerConfiguration vector is empty !");

	const ssize_t	ipv6EndPos = host.find_last_of(']');
	const ssize_t	portPos = host.find_last_of(':');
	if (portPos != -1 && portPos > ipv6EndPos)
		host.erase(portPos);

	std::vector<ServerConfiguration>::const_iterator serverIt;
	for (serverIt = _serverConfs.begin(); serverIt != _serverConfs.end(); serverIt++)
	{
		const std::vector<std::string>	serverNames = serverIt->getServerNames();
		if (std::find(serverNames.begin(), serverNames.end(), host) != serverNames.end())
			return (*serverIt);
	}
	return (_serverConfs[0]);
}

void	RequestHandler::executeRequest(Response &response, RequestContext& requestContext)
{
	if (_state != REQUEST_EMPTY_LINE)
		return ;

	const std::string* host = _request.getHeaders().getUniqueHeader("host");
	if (host == NULL)
	{
		response.setResponse(HTTP_BAD_REQUEST);
		_state = REQUEST_DONE;
		return ;
	}
	const ServerConfiguration	&serverConf = getServerConfiguration(*host);
	switch (_request.getMethod())
	{
		case GET: {
			GetRequest	getRequest(_request.getRequestTarget(), serverConf, *host, requestContext);
			response.setResponse(getRequest);
			_request.setFdData(&getRequest.getInFd());
			break;
		}
		case PUT: {
			PutRequest	putRequest(_request.getRequestTarget(), serverConf, *host, requestContext);
			response.setResponse(putRequest);
			_request.setFdData(&putRequest.getInFd());
			break;
		}
		case DELETE: {
			DeleteRequest	deleteRequest(_request.getRequestTarget(), serverConf, *host, requestContext);
			response.setResponse(deleteRequest);
			_request.setFdData(&deleteRequest.getInFd());
			break;
		}
		case POST: {
			PostRequest	postRequest(_request.getRequestTarget(), serverConf, *host, requestContext);
			response.setResponse(postRequest);
			_request.setFdData(&postRequest.getInFd());
			break;
		}
		default:
			throw std::logic_error("executeRequest called with a request method invalid !");
	}
	const Status * const	status = response.getStatus();
	if (status && status->isOfType(STATUS_ERROR))
		_state = REQUEST_DONE;
	else
		_state = REQUEST_BODY;
}
