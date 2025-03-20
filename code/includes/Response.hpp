#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <stdint.h>  	// for uint16_t
# include <iostream>  	// for ostream
# include <map>       	// for map
# include <string>    	// for string, basic_string

# include "ABody.hpp"	// for ABody

class	ARequestType;

/**
 * @brief The classe that stores all the data that will be sent to the client.
 * This class should be converted into a RawResponse to be used.
 * The creation of this class is not performant but displays the response variable
 * more clearly.
 * This class takes responsability for closing the fd.
 * The use of this class isn't mandatory because it has few uses and costs performances.
 */
class Response
{
private:
	/**
	 * @brief The first line of the response.
	 */
	struct statusLineResponse
	{
		/**
		 * @brief The return code [100, 600[
		 */
		uint16_t	statusCode;
		/**
		 * @brief The text associated with the status code. Each status code has
		 * a single status line that corresponds.
		 */
		std::string	statusText;
	}									_statusLine;
	std::map<std::string, std::string>	_headers;
	/**
	 * @brief The file descriptor of the body.If there is no body, this variable
	 * is set to -1.
	 */
	int									_bodySrcFd;
	bool								_isBlocking;
	ABody								*_body;

	Response(const Response& ref);

	Response&	operator=(const Response& response);

	void										addDefaultHeaders(void);
public:
	Response(void);
	~Response(void);

	void										setResponse(int code);
	void										setResponse(ARequestType *ARequestType, int socketFd);
	void										reset();

	uint16_t									getStatusCode(void) const;
	const std::string							&getStatusText(void) const;
	const std::string							*getHeader(const std::string &key) const;
	const std::map<std::string, std::string>	&getHeaderMap(void) const;
	bool										getIsBlocking(void) const;
};

std::ostream & operator<<(std::ostream & o, Response const & rhs);

#endif // !RESPONSE_HPP
