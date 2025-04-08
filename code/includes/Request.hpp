#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <cstring>       		// for size_t
# include <iostream>      		// for ostream
# include <map>           		// for map
# include <string>        		// for string, basic_string

# include "EMethods.hpp"  		// for EMethods
# include "SharedResource.hpp"	// for SharedResource

class ABody;
class ServerConfiguration;

/**
 * @brief the class that stores all the data send by the client.
 */
class Request
{
private:
	typedef std::map<std::string, std::string>	Headers;
	/**
	 * @brief The first line of the clients request.
	 * @note It doesn't has a protocol variable because
	 * we only accept HTTP/1.1
	 */
	struct statusLineRequest
	{
		/**
		 * @brief The method requested by the client.
		 */
		EMethods			method;
		/**
		 * @brief A path on the element the methods is applied to.
		 */
		std::string			requestTarget;
	}						_statusLine;
	Headers					_headers;
	/**
	 * @brief A SharedResource on the fd in which the request body will
	 * be written. It is the same value as the fd in the _body.
	 */
	SharedResource<int>		_bodyDestFd;
	/**
	 * @brief True if the _bodyDestFd is a blocking fd (socket, pipe).
	 * It should be set to true even if the fd has been flagged by O_NONBLOCK
	 */
	bool					_isBlocking;
	/**
	 * @brief A SharedResource on the body of the request, could be a sized body
	 * a chunked body ...
	 */
	SharedResource<ABody*>	_body;

	/**
	 * @brief parse the method and set the _statusLine.method variable.
	 * It check that the method is in uppercase and is not empty.
	 *
	 * @param begin The beginning of the method.
	 * @param end the position just after the last character of the method.
	 * @return It returns the http error status if an error happened, or HTTP_OK,
	 * if the function succeeds.
	 */
	int		parseMethod(const char *begin, const char *end);
	/**
	 * @brief parse a number from the protocol, either the major or minor.
	 * This number has to be only composed by digit and be in the range of long.
	 *
	 * @param begin The beginning of the number
	 * @param end the position just after the last character of the number.
	 * It HAS to be a '.' or '\r' or the function could segfault.
	 * @return The number parsed, or -1 if it is a bad request.
	 */
	long	parseProtocolNumber(const char *begin, const char *end);
	/**
	 * @brief parse the protocol and verify that the version major and minor
	 * are respectively equal to PROTOCOL_MAJOR and PROTOCOL_MINOR.
	 * It also checks that the protocol begin with HTTP/ or HTTPS/.
	 *
	 * @param begin The beginning of the protocol
	 * @param end The end of the status line, it HAS to be a '\r' character
	 * otherwise, the function could segfault.
	 * @return It returns the http error status if an error happened, or HTTP_OK,
	 * if the function succeeds.
	 */
	int		parseProtocol(const char *begin, const char *end);

public:
	void	reset();
	Body	*getBody();
	int		parseStatusLine(char *start, char *end);
	int		parseHeader(char *start, char *end);
};

std::ostream & operator<<(std::ostream & o, Request const & rhs);

#endif // !REQUEST_HPP
