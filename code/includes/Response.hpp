#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <string>
# include <map>

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
	std::string	firstPart;
	/**
	 * @brief The number of character from firstPart that has been read. It should
	 * be in range [0, firstPart.size()]
	 */
	size_t		rd;
	/**
	 * @brief The file descriptor of the body.If there is no body, or it has already
	 * been included in firstPart, this variable is set to -1.
	 */
	int			bodyFd;
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
	}									statusLine;
	std::map<std::string, std::string>	headers;
	/**
	 * @brief The file descriptor of the body.If there is no body, this variable
	 * is set to -1.
	 */
	int									bodyFd;
};

#endif // !RESPONSE_HPP