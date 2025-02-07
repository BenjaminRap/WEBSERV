#ifndef SERVERCONFIGURATION_HPP
# define SERVERCONFIGURATION_HPP

# include <map>
# include <stdint.h>

# include "Route.hpp"

/// @brief The configuration specific for each server
class ServerConfiguration
{
public :

	ServerConfiguration(std::vector<std::string> serverNames, \
						std::map<unsigned short, std::string> \
						errorPages, size_t maxClientBodySize, \
						std::map<std::string, Route> routes, \
						std::string root);
	ServerConfiguration(ServerConfiguration const &src);
    ServerConfiguration    &operator=(ServerConfiguration const &src);
	~ServerConfiguration(void);

	const std::string							&getRoot(void) const;
	const std::vector<std::string>				&getServerNames(void) const;
	const std::string							&getErrorPage(unsigned short error) const;
	const std::map<unsigned short, std::string>	&getErrorPages(void) const;
	const size_t								&getMaxClientBodySize(void) const;


private :
	std::vector<std::string>				serverNames;
	/// @brief ushort : error code, std::string, page path
	/// Associate an error with a page, returned to the client.
	std::map<unsigned short, std::string>	errorPages;
	size_t									maxClientBodySize;
	/// @brief string : std::string location, class Route,
	/// Associate a location, with the corresponding Route.
	std::map<std::string, Route>			routes;
	std::string								root;

	ServerConfiguration(void);
};

std::ostream & operator<<(std::ostream & o, ServerConfiguration const & rhs);

#endif // !SERVER_CONFIGURATION_HPP
