#include <stddef.h>                 // for NULL
#include <algorithm>                // for find
#include <iostream>                 // for basic_ostream, cout, operator<<
#include <stdexcept>                // for logic_error
#include <string>                   // for basic_string, string, operator==
#include <vector>                   // for vector

#include "DeleteRequest.hpp"        // for DeleteRequest
#include "EMethods.hpp"             // for EMethods
#include "GetRequest.hpp"           // for GetRequest
#include "PutRequest.hpp"           // for PutRequest
#include "Request.hpp"              // for Request, operator<<
#include "RequestHandler.hpp"       // for RequestHandler, RequestState
#include "Response.hpp"             // for operator<<, Response (ptr only)
#include "ServerConfiguration.hpp"  // for ServerConfiguration

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

void	RequestHandler::executeRequest(Response &response)
{
	if (_state != REQUEST_EMPTY_LINE)
		return ;

	const ServerConfiguration	serverConfiguration = getServerConfiguration();
	std::cout << _request << '\n';
	switch (_request.getMethod())
	{
		case GET: {
			const GetRequest	getRequest(_request.getRequestTarget(), serverConfiguration);
			break;
		}
		case POST: {
			break;
		}
		case PUT: {
			const PutRequest	putRequest(_request.getRequestTarget(), serverConfiguration);
			break;
		}
		case DELETE: {
			const DeleteRequest	deleteRequest(_request.getRequestTarget(), serverConfiguration);
			break;
		}
		default:
			throw std::logic_error("executeRequest called with a request method invalid !");
	}
	std::cout << response << std::endl;
}
