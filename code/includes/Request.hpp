#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <cstring>
# include <map>
# include <iostream>
# include <sstream>
# include <iomanip>
# include <algorithm>

# include "Methods.hpp"
# include "Body.hpp"

# define FWS "\t\n\v\f\r "

/**
 * @brief the class that stores all the data send by the client, it shouldn't be
 * changed after construction.
 */
class Request
{
private:
	/**
	 * @brief The first line of the clients request.
	 */
	struct statusLineRequest
	{
		/**
		 * @brief The method requested by the client.
		 */
		EMethods						_method;
		/**
		 * @brief A path on the element the methods is applied to.
		 */
		std::string						_requestTarget;
		/**
		 * @brief The version of HTTP, in our case : HTTP/1.1.
		 */
	}									_statusLine;
	std::map<std::string, std::string>	_headers;
	/**
	 * @brief The body of the client socket if there is a remaining body.
	 * If there is no remaining body, this variable is set to NULL.
	 */
	Body								*_body;

public:

	Request(void);
	~Request(void);

	void	reset();
	Body				*getBody() const;
	EMethods			getMethod(void) const;
	const std::string	&getRequestTarget(void) const;
	const std::string	*getHeader(const std::string &key) const;
	const std::map<std::string, std::string>	&getHeaderMap(void) const;
	int		parseStatusLine(const char *line, size_t lineLength);
	int		parseHeader(const char *line, size_t lineLength);
};

std::ostream & operator<<(std::ostream & o, Request const & rhs);

#endif // !REQUEST_HPP