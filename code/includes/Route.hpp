#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <vector>
# include <string>
# include <iostream>

# include "EMethods.hpp"

/// @brief This structure describe a redirection.
/// A redirection has a status code, in the range [300,400[ : ex : 301
/// It also has the url in which it redirects the request. ex : https://other-url
struct SRedirection
{
public:
	unsigned short	responseStatusCode;
	std::string		url;
};

/// @brief This class describe a route, a server can have multiples routes or none.
class Route
{
public:
	Route(	const std::vector<EMethods> &acceptedMethods, \
			const SRedirection &redirection, \
			const std::vector<std::string> &index, \
			const bool &auto_index, \
			const std::string &root, \
			const std::string &cgiFileExtension, \
			const bool	&acceptUploads);
	Route(Route const &src);
	~Route(void);

	const std::vector<EMethods>&	getAcceptedMethods(void) const;
	const SRedirection&				getRedirection(void) const;
	const std::vector<std::string>&	getIndex(void) const;
	bool							getAutoIndex(void) const;
	const std::string&				getRoot(void) const;
	const std::string&				getCgiFileExtension(void) const;
	bool							getAcceptUploads(void) const;
	void							setIndex(const std::vector<std::string> &v);

private:
	std::vector<EMethods>		acceptedMethods;
	SRedirection				redirection;
	std::vector<std::string>	index;
	bool						autoIndex;
	/// @brief Define a directory or a file from where the file should be searched,
	/// (if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
	/// /tmp/www/pouic/toto/pouet).
	std::string					root;
	/// @brief If a get method ask for a directory, return a list of all elements
	/// in this directory.
	bool						directoryListing;
	/// @brief The file returned if the get method ask for a directory.
	std::string					directoryFile;
	/// @brief if a file, at this route has this file extension, it will call the
	/// cgi.
	std::string					cgiFileExtension;
	/// @brief if a file, at this route has uploads
	bool						acceptUploads;

	Route(void);
	Route    &operator=(Route const &src);
};

std::ostream & operator<<(std::ostream & o, Route const & rhs);

#endif // !ROUTE_HPP

