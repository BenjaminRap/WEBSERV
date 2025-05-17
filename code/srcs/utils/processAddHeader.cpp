#include "ARequestType.hpp"
#include "RequestHandler.hpp"
#include "Response.hpp"

void	processAddHeader(Headers &response_headers, ARequestType &req, const ServerConfiguration &config)
{
	for (std::map< std::string, std::map< std::string, std::map< bool, Route * > > >::const_iterator it \
	= config.getAddHeader().begin(); it != config.getAddHeader().end(); ++it)
	{
		if ((it->second.begin()->second.begin()->second == NULL || it->second.begin()->second.begin()->second == req.getRoute()) && \
		(req.getCode() < 400 || it->second.begin()->second.begin()->first))
		{
			response_headers[it->first] = it->second.begin()->first;
		}
	}
}
