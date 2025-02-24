#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <string>
# include <map>
# include <iostream>

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
	Body								*_body;

	Request(void);

public:
	Request(std::string line);
	~Request(void);

	void	reset();
	Body	*getBody() const;
	const std::string	&getMethod(void) const;
	const std::string	&getRequestTarget(void) const;
	const std::string	&getProtocol(void) const;
	const std::string	*getHeader(const std::string &key) const;
	const std::map<std::string, std::string>	&getHeaderMap(void) const;
	// int		parseStatusLine(char *line, size_t lineLength);
	int		parseHeader(const std::string &s);
};

std::ostream & operator<<(std::ostream & o, Request const & rhs);

class RequestException : public std::exception
{
	public:
		RequestException(std::string message) : message(message)
		{
			error = errorMsg();
		}

		std::string errorMsg() const
		{
			return ("Error parsing request: " + message);
		}

		virtual const char* what() const throw()
		{
			return (error.c_str());
		}
		virtual ~RequestException() throw() {}

	private:
		std::string error;
		std::string message;
};

#endif // !REQUEST_HPP