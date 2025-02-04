#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>

# include "Body.hpp"

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
		std::string						_method;
		/**
		 * @brief A path on the element the methods is applied to.
		 */
		std::string						_requestTarget;
		/**
		 * @brief The version of HTTP, in our case : HTTP/1.1.
		 */
		std::string						_protocol;
	}									_statusLine;
	std::map<std::string, std::string>	_headers;
	/**
	 * @brief The body of the client socket if there is a remaining body.
	 * If there is no remaining body, this variable is set to NULL.
	 */
	Body								*body;
};

#endif // !REQUEST_HPP