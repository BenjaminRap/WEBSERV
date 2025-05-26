#ifndef ROUTE_HPP
# define ROUTE_HPP

# include <vector>
# include <string>
# include <iostream>

# include "EMethods.hpp"

/**
 * @class SRedirection
 * @brief This structure describe a redirection.
 * A redirection has a status code, in the range [300,400[ : ex : 301
 * It also has the url in which it redirects the request. ex : https://other-url
 *
 */
struct SRedirection
{
public:
	unsigned short	responseStatusCode;
	std::string		url;
};

/**
 * @class Route
 * @brief This class describe a route, a server can have multiples routes or none.
 *
 */
class Route
{
public:
	Route
	(
		const std::vector<EMethods> &acceptedMethods,
		const SRedirection &redirection,
		size_t maxClientBodySize,
		const std::vector<std::string> &index,
		const bool &auto_index,
		const std::string &root,
		const std::string &cgiFileExtension,
		const std::string &cgiInterpreter
	);
	Route(Route const &src);
	~Route(void);

	const std::vector<EMethods>&	getAcceptedMethods(void) const;
	const SRedirection&				getRedirection(void) const;
	const std::vector<std::string>&	getIndex(void) const;
	bool							getAutoIndex(void) const;
	const std::string&				getRoot(void) const;
	const std::string&				getCgiFileExtension(void) const;
	const std::string&				getCgiInterpreter(void) const;
	void							setIndex(const std::vector<std::string> &v);
	const size_t&					getMaxClientBodySize(void) const;

private:
	/**
	 * @brief A list of all methods this route accept. For example,
	 * if this route acceptedMethods are GET and POST, and the request
	 * is a delete request, we will send a 405.
	 */
	std::vector<EMethods>		_acceptedMethods;
	/**
	 * @brief A structure containing an url and a status code.
	 * If a request, ask for this route, we redirect it to the url
	 * and returns the response status code.
	 */
	SRedirection				_redirection;
	/**
	 * @brief The maximum size of a request body. If a request
	 * body is greater than that, a HTTP_CONTENT_TOO_LARGE.
	 */
	size_t						_maxClientBodySize;
	/**
	 * @brief A vector of all the index names. That means the default
	 * page that will be shown if the user ask for a folder.
	 */
	std::vector<std::string>	_index;
	/**
	 * @brief If a get method ask for a directory nad this variable is set
	 * to true, return a page created from the list of all elements in this directory.
	 */
	bool						_autoIndex;
	/**
	 * @brief Define a directory or a file from where the file should be searched,
	 * (if url /kapouet is rooted to /tmp/www, url /kapouet/pouic/toto/pouet is
	 * /tmp/www/pouic/toto/pouet).
	 */
	std::string					_root;
	/**
	 * @brief if a file, at this route has this file extension, it will execute it
	 * and returns the results of the cgi, instead of returning the file.
	 */
	std::string					_cgiFileExtension;
	std::string					_cgiInterpreter;
	/**
	 * @brief if a file, at this route accept uploads through the POST or PUT
	 * requests.
	 */
	bool						_acceptUploads;

	Route(void);
	Route    &operator=(Route const &src);
};

std::ostream & operator<<(std::ostream & o, Route const & rhs);

#endif // !ROUTE_HPP

