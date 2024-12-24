#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <vector>
# include <string>

# include "Methods.hpp"

/// @brief This structure describe a redirection.
/// A redirection has a status code, in the range [300,400[ : ex : 301
/// It also has the url in which it redirects the request. ex : https://other-url
struct SRedirection
{
	ushort		responseStatusCode;
	std::string	url;
};

/// @brief This structure describe the uploads behaviour
/// If this route accept the uploads, the post request will create a file at $path
/// with the content of the body.
struct SUploads
{
	bool		acceptUploads;
	std::string	path;
};

/// @brief This class describe a route, a server can have multiples routes or none.
class Route
{
private:
	std::vector<EMethods>	acceptedMethods;
	SRedirection			redirection;
	std::string				root;
	bool					directoryListing;
	std::string				directoryFile;
	std::string				cgiFileExtension;
	SUploads				uploads;
};

#endif // !ROUTE_HPP

