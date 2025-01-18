#ifndef RAW_RESPONSE_HPP
# define RAW_RESPONSE_HPP

# include <string>
# include <sys/types.h>

# define RESPONSE_BUFFER_SIZE 1024

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
	ssize_t	writeToFd(int fd, char (&buffer)[RESPONSE_BUFFER_SIZE], size_t bufferLength);
};

#endif // !RAW_RESPONSE_HPP