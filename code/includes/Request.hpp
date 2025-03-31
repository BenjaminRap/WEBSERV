#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <cstring>       		// for size_t
# include <iostream>      		// for ostream
# include <map>           		// for map
# include <string>        		// for string, basic_string

# include "EMethods.hpp"  		// for EMethods
# include "SharedResource.hpp"	// for SharedResource

class ABody;

# define FWS "\t\n\v\f\r "

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
		EMethods						method;
		/**
		 * @brief A path on the element the methods is applied to.
		 */
		std::string						requestTarget;
	}									_statusLine;
	std::map<std::string, std::string>	_headers;
	/**
	 * @brief A SharedResource on the fd in which the request body will
	 * be written. It is the same value as the fd in the _body.
	 */
	SharedResource<int>					_bodyDestFd;
	/**
	 * @brief True if the _bodyDestFd is a blocking fd (socket, pipe).
	 * It should be set to true even if the fd has been flagged by O_NONBLOCK
	 */
	bool								_isBlocking;
	/**
	 * @brief A SharedResource on the body of the request, could be a sized body
	 * a chunked body ...
	 */
	SharedResource<ABody*>				_body;

public:

	Request(void);
	~Request(void);

	/**
	 * @brief Reset all the attributes of this instance, as they were after the
	 * default constructor call.
	 */
	void										reset();
	/**
	 * @brief Parse the line and set the method and requestTare=get attributes.
	 *
	 * @param line the status line. It doesn't have to be null terminated.
	 * @param end the position just after the last character.
	 * @return the http status corresponding to the error (HTTP_BAD_REQUEST ...),
	 * or HTTP_OK if there is no errors.
	 */
	int											parseStatusLine(const char *line, const char *end);
	/**
	 * @brief Parse the line and add a header to the _headers map.
	 *
	 * @param line a line containing key:value. It doesn't have to be null terminated.
	 * @param end the position just after the last character.
	 * @return the http status corresponding to the error (HTTP_BAD_REQUEST ...),
	 * or HTTP_OK if there is no errors.
	 */
	int											parseHeader(const char *line, const char *end);
	/**
	 * @brief Set this instance _bodyDestFd, _isBlocking and _body, depending on
	 * the headers. For example, a Content-Length header means a SizedBody.
	 *
	 * @param destFd The fd in which the body will write, (ex: a file, a pipe ...)
	 * @return the http status corresponding to the error (HTTP_BAD_REQUEST ...),
	 * or HTTP_OK if there is no errors.
	 */
	int											setBodyFromHeaders(SharedResource<int> destFd);

	ABody*										getBody() const;
	EMethods									getMethod(void) const;
	const std::string&							getRequestTarget(void) const;
	const std::string*							getHeader(const std::string &key) const;
	const std::map<std::string, std::string>&	getHeaderMap(void) const;
	bool										getIsBlocking(void) const;
};

std::ostream & operator<<(std::ostream & o, Request const & rhs);

#endif // !REQUEST_HPP
