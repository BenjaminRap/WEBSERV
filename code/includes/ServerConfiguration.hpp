#ifndef SERVER_CONFIGURATION_HPP
# define SERVER_CONFIGURATION_HPP

# include <string>
# include <map>
# include <vector>
# include <stdint.h>
# include <bits/sockaddr.h>
# include <netinet/in.h>

# include "Route.hpp"

union sockaddr_in_u
{
	sockaddr_in		ipv4;
	sockaddr_in6	ipv6;
};

struct Host
{
	sa_family_t		family;
	sockaddr_in_u	addr;
	

	Host(in_addr_t addrIPV4, in_port_t port);
	Host(uint8_t (&addrIPV6)[16], in_port_t port);

	bool	operator<(const Host &host) const;
};


/// @brief The configuration specific for each server
class ServerConfiguration
{
public:
	std::vector<std::string>				serverNames;
	/// @brief ushort : error code, std::string, page path
	/// Associate an error with a page, returned to the client.
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize;
	std::vector<Route>						routes;

};

#endif // !SERVER_CONFIGURATION_HPP
