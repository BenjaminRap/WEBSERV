#include "ARequestType.hpp"
#include "RequestHandler.hpp"
#include "Response.hpp"

void	processAddHeader(Headers &response_headers, ARequestType &req, const ServerConfiguration &config)
{
	const std::map< std::string, std::pair<std::string, bool> > &rootAddHeader = config.getAddHeader();
	const uint16_t code = req.getCode();

	for (std::map< std::string, std::pair<std::string, bool> >::const_iterator it = rootAddHeader.begin(); it != rootAddHeader.end(); ++it)
	{
		if (it->second.second == true || code < 400)
			response_headers.insert(std::make_pair(it->first, it->second.first));
	}

	const std::map< std::string, std::pair<std::string, bool> > &routeAddHeader = req.getRoute()->getAddHeader();

	for (std::map< std::string, std::pair<std::string, bool> >::const_iterator it = routeAddHeader.begin(); it != routeAddHeader.end(); ++it)
	{
		if (it->second.second == true || code < 400)
			response_headers.insert(std::make_pair(it->first, it->second.first));
	}
}
