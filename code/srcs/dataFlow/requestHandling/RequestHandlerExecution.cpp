#include "RequestHandler.hpp"
#include "GetRequest.hpp"
#include "PutRequest.hpp"
#include "DeleteRequest.hpp"

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
	_state = REQUEST_DONE;
	std::cout << _request << '\n';
	switch (_request.getMethod())
	{
		case GET: {
			const GetRequest	getRequest(_request.getRequestTarget(), serverConfiguration);
			response.setResponse(getRequest.getCode(), getRequest.getRedirection());
			break;
		}
		case POST: {
			break;
		}
		case PUT: {
			const PutRequest	putRequest(_request.getRequestTarget(), serverConfiguration);
			response.setResponse(putRequest.getCode(), putRequest.getRedirection());
			_request.setBodyFromHeaders(putRequest.getFd(), false);
			_state = REQUEST_BODY;
			break;
		}
		case DELETE: {
			const DeleteRequest	deleteRequest(_request.getRequestTarget(), serverConfiguration);
			response.setResponse(deleteRequest.getCode(), deleteRequest.getRedirection());
			break;
		}
		default:
			throw std::logic_error("executeRequest called with a request method invalid !");
	}
	std::cout << response << std::endl;
}


