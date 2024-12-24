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
public:
	unsigned short	responseStatusCode;
	std::string		url;
};

/// @brief This structure describe the uploads behaviour
/// If this route accept the uploads, the post request will create a file at $path
/// with the content of the body.
struct SUploads
{
public:
	bool		acceptUploads;
	std::string	path;
};

/// @brief This class describe a route, a server can have multiples routes or none.
class Route
{
public:
	std::vector<EMethods>	acceptedMethods;
	SRedirection			redirection;
	/// @brief Define a directory or a file from where the file should be searched,
	/// (if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
	/// /tmp/www/pouic/toto/pouet).

	std::string				root;
	/// @brief If a get method ask for a directory, return a list of all elements
	/// in this directory.
	bool					directoryListing;
	/// @brief The file returned if the get method ask for a directory.
	std::string				directoryFile;
	/// @brief if a file, at this route has this file extension, it will call the
	/// cgi.
	std::string				cgiFileExtension;
	SUploads				uploads;
};

#endif // !ROUTE_HPP

