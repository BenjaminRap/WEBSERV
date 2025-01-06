#ifndef SERVER_CONFIGURATION_HPP
# define SERVER_CONFIGURATION_HPP

# include <string>
# include <map>
# include <vector>
#include <stdint.h>

# include "Route.hpp"

/// @brief The configuration specific for each server
class ServerConfiguration
{
public:
	uint32_t								host;
	uint16_t								port;
	std::vector<std::string>				serverNames;
	/// @brief ushort : error code, std::string, page path
	/// Associate an error with a page, returned to the client.
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize;
	std::vector<Route>						routes;
};

#endif // !SERVER_CONFIGURATION_HPP
