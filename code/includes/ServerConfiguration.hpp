#ifndef SERVER_CONFIGURATION_HPP
# define SERVER_CONFIGURATION_HPP

# include <string>
# include <map>
# include <vector>
# include <stdint.h>
# include <bits/sockaddr.h>

# include "Route.hpp"

struct Host
{
	uint32_t	address;
	sa_family_t	family;
	uint16_t	port;

	Host(uint32_t address, sa_family_t family, uint16_t port);

	bool	operator<(const Host &host) const;
};


/// @brief The configuration specific for each server
class ServerConfiguration
{
public:
	Host									*host;
	std::vector<std::string>				serverNames;
	/// @brief ushort : error code, std::string, page path
	/// Associate an error with a page, returned to the client.
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize;
	std::vector<Route>						routes;

};

#endif // !SERVER_CONFIGURATION_HPP
