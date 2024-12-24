#ifndef SERVER_CONFIGURATION_HPP
# define SERVER_CONFIGURATION_HPP

# include <string>
# include <map>
# include <vector>

# include "Route.hpp"

/// @brief The configuration specific for each server
class ServerConfiguration
{
private:
	std::string						host;
	ushort							port;
	std::vector<std::string>		serverNames;
	std::map<ushort, std::string>	errorPages;
	size_t							maxClientBodySize;
	std::vector<Route>				routes;
};

#endif // !SERVER_CONFIGURATION_HPP
