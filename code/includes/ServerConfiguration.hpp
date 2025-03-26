#ifndef SERVERCONFIGURATION_HPP
# define SERVERCONFIGURATION_HPP

# include <stddef.h>   // for size_t
# include <iosfwd>     // for ostream
# include <map>        // for map
# include <string>     // for string, basic_string
# include <vector>     // for vector
# include <stdint.h>   // for uint16_t

# include "Route.hpp"  // for Route

/// @brief The configuration specific for each server
class ServerConfiguration
{
public :

	ServerConfiguration(const std::vector<std::string> &serverNames, \
						const std::map<unsigned short, std::string> &errorPages, \
						const size_t &maxClientBodySize, \
						const std::map<std::string, Route> &routes, \
						const std::string &root, \
						const std::vector<std::string> &index);
	ServerConfiguration(ServerConfiguration const &src);
    ServerConfiguration    &operator=(ServerConfiguration const &src);
	~ServerConfiguration(void);

	const std::string							&getRoot(void) const;
	const std::vector<std::string>				&getServerNames(void) const;
	const std::string							&getErrorPage(unsigned short error) const;
	const std::map<unsigned short, std::string>	&getErrorPages(void) const;
	const size_t								&getMaxClientBodySize(void) const;
	const std::map<std::string, Route>			&getRoutes(void) const;
	const Route									*getRouteFromPath(const std::string &path) const;
	const std::string							getLocation(const std::string &loc) const;
	const std::vector<std::string>				&getIndex(void) const;

private :
	std::vector<std::string>				serverNames;
	/// @brief ushort : error code, std::string, page path
	/// Associate an error with a page, returned to the client.
	std::map<uint16_t, std::string>	errorPages;
	size_t									maxClientBodySize;
	/// @brief string : std::string location, class Route,
	/// Associate a location, with the corresponding Route.
	std::map<std::string, Route>			routes;
	std::string								root;
	std::vector<std::string>				index;

	ServerConfiguration(void);
};

std::ostream & operator<<(std::ostream & o, ServerConfiguration const & rhs);

#endif // !SERVER_CONFIGURATION_HPP
