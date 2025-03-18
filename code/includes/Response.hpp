#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <iostream>
# include <map>
# include <ctime>
# include <stdint.h>

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
		 * @brief The version of http we are using : in our case HTTP/1.1.
		 */
		std::string	protocol;
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
	int									_bodyFd;
public:
	void										setResponse(int code, const std::string &redirection);
	void										reset();

	uint16_t									getStatusCode(void) const;
	const std::string							&getStatusText(void) const;
	const std::string							&getProtocol(void) const;
	const std::string							*getHeader(const std::string &key) const;
	const std::map<std::string, std::string>	&getHeaderMap(void) const;

};

std::ostream & operator<<(std::ostream & o, Response const & rhs);

#endif // !RESPONSE_HPP
