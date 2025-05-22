#ifndef SERVERCONFIGURATION_HPP
# define SERVERCONFIGURATION_HPP

# include <stddef.h>   // for size_t
# include <iosfwd>     // for ostream
# include <map>        // for map
# include <string>     // for string, basic_string
# include <vector>     // for vector
# include <stdint.h>   // for uint16_t

# include "Route.hpp"  // for Route

/**
 * @class ServerConfiguration
 * @brief The configuration specific for each server
 *
 */
class ServerConfiguration
{
public :

	ServerConfiguration
	(
		const std::vector<std::string> &serverNames,
		const std::map<unsigned short, std::string> &errorPages,
		const size_t &maxClientBodySize,
		const std::map<std::string, Route> &routes,
		const std::string &root,
		const std::vector<std::string> &index
	);
	ServerConfiguration(ServerConfiguration const &src);
	~ServerConfiguration(void);

	const std::string&							getRoot(void) const;
	const std::vector<std::string>&				getServerNames(void) const;
	const std::string*							getErrorPage(uint16_t error) const;
	const std::map<uint16_t, std::string>&		getErrorPages(void) const;
	const size_t&								getMaxClientBodySize(void) const;
	const std::map<std::string, Route>&			getRoutes(void) const;
	const std::pair<const std::string, Route>*	getRouteFromPath(const std::string &path) const;
	const std::vector<std::string>&				getIndex(void) const;

private :
	/**
	 * @brief A vector of the differents server names this server
	 * can be referred to. It will be sued to determine if the request
	 * ask for this ServerConfiguration or not.
	 */
	std::vector<std::string>				_serverNames;
	/**
	 * @brief A map between an error and an error page path.
	 * When having an error in this ServerConfiguration,
	 * the errorPage will be sent instead of the default one.
	 */
	std::map<uint16_t, std::string>			_errorPages;
	/**
	 * @brief The maximum size of a request body. If a request
	 * body is greater than that, a HTTP_CONTENT_TOO_LARGE.
	 */
	size_t									_maxClientBodySize;
	/**
	 * @brief A map with : route url:route class.
	 */
	std::map<std::string, Route>			_routes;
	/**
	 * @brief Define a directory or a file from where the file should be searched,
	 * (if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
	 * /tmp/www/pouic/toto/pouet).
	 */
	std::string								_root;
	/**
	 * @brief A vector of all the index names. That means the default
	 * page that will be shown if the user ask for a folder.
	 */
	std::vector<std::string>				_index;

	ServerConfiguration(void);
	ServerConfiguration    &operator=(ServerConfiguration const &src);
};

std::ostream & operator<<(std::ostream & o, ServerConfiguration const & rhs);

#endif // !SERVER_CONFIGURATION_HPP
