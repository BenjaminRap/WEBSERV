#include <algorithm>                // for find
#include <cstdio>                   // for NULL
#include <stdexcept>                // for logic_error
#include <string>                   // for basic_string, string, operator==
#include <vector>                   // for vector

#include "ARequestType.hpp"         // for ARequestType
#include "DeleteRequest.hpp"        // for DeleteRequest
#include "EMethods.hpp"             // for EMethods
#include "GetRequest.hpp"           // for GetRequest
#include "PutRequest.hpp"           // for PutRequest
#include "Request.hpp"              // for Request, operator<<
#include "RequestHandler.hpp"       // for RequestHandler, RequestState
#include "Response.hpp"             // for Response
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "requestStatusCode.hpp"    // for HTTP_OK

const ServerConfiguration&	RequestHandler::getServerConfiguration(const std::string& host) const
{
	if (_serverConfs.size() == 0)
		throw std::logic_error("The ServerConfiguration vector is empty !");

	std::vector<ServerConfiguration>::const_iterator serverIt;

	for (serverIt = _serverConfs.begin(); serverIt != _serverConfs.end(); serverIt++)
	{
		const std::vector<std::string>	serverNames = serverIt->getServerNames();
		if (std::find(serverNames.begin(), serverNames.end(), host) != serverNames.end())
			return (*serverIt);
	}
	return (_serverConfs[0]);
}

void	RequestHandler::processRequestResult
(
	ARequestType &requestResult,
	Response &response
)
{
	{
		const int status = _request.setBodyFromHeaders(requestResult.getInFd(), requestResult.getConfig());
		if (status != HTTP_OK)
		{
			response.setResponse(status);
			_state = REQUEST_DONE;
			return ;
		}
	}

	response.setResponse(requestResult);
	_state = REQUEST_BODY;
}


void	RequestHandler::executeRequest(Response &response, EPollHandler& ePollHandler)
{
	if (_state != REQUEST_EMPTY_LINE)
		return ;

	const std::string* host = _request.getHeaders().getHeader("host");
	if (host == NULL)
	{
		response.setResponse(HTTP_BAD_REQUEST);
		_state = REQUEST_DONE;
		return ;
	}
	const ServerConfiguration	serverConfiguration = getServerConfiguration(*host);
	switch (_request.getMethod())
	{
		case GET: {
			GetRequest	getRequest(_request.getRequestTarget(), serverConfiguration, ePollHandler);
			processRequestResult(getRequest, response);
			break;
		}
		case PUT: {
			PutRequest	putRequest(_request.getRequestTarget(), serverConfiguration, ePollHandler);
			processRequestResult(putRequest, response);
			break;
		}
		case DELETE: {
			DeleteRequest	deleteRequest(_request.getRequestTarget(), serverConfiguration, ePollHandler);
			processRequestResult(deleteRequest, response);
			break;
		}
		default:
			throw std::logic_error("executeRequest called with a request method invalid !");
	}
}
