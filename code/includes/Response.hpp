#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>
# include <sys/types.h>
# include <stdint.h>

/**
 * @brief This class stores all the data that will be sent to the client as a
 * response, compacted in a single string and a fd for the body. This class take
 * responsability for the closing of the fd.
 * It moves the data from the file to the socket like a stream, without allocation.
 * This class is designed to be minimalistic.
 */
class RawResponse
{
private:
	/**
	 * @brief A string containing the first part of the response, that means :
	 * the status line, the headers, the blank line and, if there is a static one,
	 * a body.
	 */
	std::string	_firstPart;
	/**
	 * @brief The number of character from _firstPart that need to be written
	 * to the socket. It should be in range [-1, firstPart.size()] : -1 meaning
	 * there was an error, 0 meaning there is nothing to read anymore.
	 */
	ssize_t		_numCharsToWrite;
	/**
	 * @brief The file descriptor of the body.If there is no body, or it has already
	 * been included in firstPart, this variable is set to -1.
	 */
	int			_bodyFd;

public:
	ssize_t	writeToFd(int fd);
};

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
		std::string	_protocol;
		/**
		 * @brief The return code [100, 600[
		 */
		uint16_t	_statusCode;
		/**
		 * @brief The text associated with the status code. Each status code has
		 * a single status line that corresponds.
		 */
		std::string	_statusText;
	}									_statusLine;
	std::map<std::string, std::string>	_headers;
	/**
	 * @brief The file descriptor of the body.If there is no body, this variable
	 * is set to -1.
	 */
	int									_bodyFd;
};

#endif // !RESPONSE_HPP