#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>

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
		std::string	_method;
		/**
		 * @brief A path on the element the methods is applied to.
		 */
		std::string	_requestTarget;
		/**
		 * @brief The version of HTTP, in our case : HTTP/1.1.
		 */
		std::string	_protocol;
	}									_statusLine;
	std::map<std::string, std::string>	_headers;
	/**
	 * @brief When reading the headers, a part of the body can be read. It can be
	 * empty, a part of the body or the entire body.
	 * A request has no body if this string is empty and the bodyFd is set to -1.
	 */
	std::string							_alreadyReadBody;
	/**
	 * @brief The file descriptor of the client socket if there is a remaining body.
	 * If there is no remaining body, this variable is set to -1.
	 */
	int									_bodyFd;
};

#endif // !REQUEST_HPP