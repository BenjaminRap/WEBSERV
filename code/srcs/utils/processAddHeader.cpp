#include "ARequestType.hpp"
#include "RequestHandler.hpp"
#include "Response.hpp"

void	processAddHeader(Headers &responseHeaders, ARequestType &req, const ServerConfiguration &config)
{
	const Route * const				route = req.getRoute();
	const std::list<ConfigHeaders>& rootAddHeader = (route) ? route->getAddHeader() : config.getAddHeader();
	const uint16_t					code = req.getCode();

	for (std::list<ConfigHeaders>::const_iterator header = rootAddHeader.begin(); header != rootAddHeader.end(); header++)
	{
		if (header->always == true || code < 400)
			responseHeaders.addHeader(header->key, header->value);
	}
}
