#include <string>
#include <errno.h>
#include <cstring>
#include <iostream>

/**
 * @brief Check if the value is the same as the error value. If true, print the
 * error prefix and the strerror(errno). It can be used like that :
 * checkError(close(fd), -1, "close() : ");
 * This way, even if we can't manage a close error, we print an error message.
 * @return Return value
 */
template <typename ReturnType>
ReturnType	checkError(ReturnType value, ReturnType error, std::string errorPrefix)
{
	if (value == error)
		std::cerr << errorPrefix << strerror(errno) << std::endl;
	return (value);
}