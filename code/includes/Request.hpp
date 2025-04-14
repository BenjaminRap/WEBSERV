#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <cstring>       		// for size_t
# include <iostream>      		// for ostream
# include <string>        		// for string, basic_string

# include "EMethods.hpp"  		// for EMethods
# include "Headers.hpp"			// for Headers
# include "SharedResource.hpp"	// for SharedResource

class ABody;
class ServerConfiguration;

/**
 * @brief the class that stores all the data send by the client.
 */
class Request
{
private:
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

	Request(void);
	~Request(void);

	/**
	 * @brief Reset all the attributes of this instance, as they were after the
	 * default constructor call.
	 */
	void				reset();
	/**
	 * @brief Parse the line and set the method and requestTare=get attributes.
	 *
	 * @param line the status line. It doesn't have to be null terminated.
	 * @param end the position just after the last character.
	 * @return the http status corresponding to the error (HTTP_BAD_REQUEST ...),
	 * or HTTP_OK if there is no errors.
	 */
	int					parseStatusLine(const char *line, const char *end);
	/**
	 * @brief Set this instance _bodyDestFd, _isBlocking and _body, depending on
	 * the headers. For example, a content-length header means a SizedBody.
	 *
	 * @param destFd The fd in which the body will write, (ex: a file, a pipe ...)
	 * @param the configuration of the server, used to check if the content-length is
	 * small enough.
	 * @return the http status corresponding to the error (HTTP_BAD_REQUEST ...),
	 * or HTTP_OK if there is no errors.
	 */
	int					setBodyFromHeaders
	(
		SharedResource<int> destFd,
		const ServerConfiguration& serverConfiguration
	);

	ABody*				getBody() const;
	Headers&			getHeaders();
	const Headers&		getHeaders() const;
	EMethods			getMethod(void) const;
	const std::string&	getRequestTarget(void) const;
	bool				getIsBlocking(void) const;
};

std::ostream & operator<<(std::ostream & o, Request const & rhs);

#endif // !REQUEST_HPP
