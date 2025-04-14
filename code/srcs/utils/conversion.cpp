#include <cerrno>	// for errno
#include <cstdlib>	// for std::strtoul
#include <limits>	// for std::numeric_limits
#include <string>	// for std::string
#include <climits>	// for ULONG_MAX
#include <sstream>	// for std::ostringstream

std::string	sizeTToString(size_t value)
{
	std::ostringstream	strStream;

	strStream << value;
	return (strStream.str());
}

/**
 * @brief Get the maximum number a long can represents.
 *
 */
long	getLongMax()
{
	static const long	longMax = std::numeric_limits<long>::max();

	return (longMax);
}

/**
 * @brief Parse the string from begin to end, and convert it to a long.
 * This function check that every characters are from the base
 * and that every character are used in the conversion.
 * This function does not set errno.
 *
 * @param begin the beginning of the string.
 * @param end the character just after the last character of the number. It should
 * not be out of bounds and should not be a character of the base, otherwise a segfault
 * could happend.
 * @param isInBase A function that returns non zero if the character is from the base, 0 otherwise.
 * @param base The base of the string
 * @return The number parsed if there is no error.
 * If an error occured, it return long max.
 * @ref getLongMax
 */
long	strToLongBase(const char *begin, const char* end, int (&isInBase)(int character), int base)
{
	for (const char* index = begin; index != end; index++)
	{
		if (!isInBase(*index))
			return (getLongMax());
	}
	const int	errnoSave = errno;

	char*		numberEnd;

	errno = 0;
	const long	number = std::strtol(begin, &numberEnd, base);
	const bool	numberTooBig = errno == ERANGE;
	errno = errnoSave;
	if (begin == numberEnd || numberTooBig || numberEnd != end)
		return (getLongMax());
	return (number);
}

/**
 * @brief Parse the string and convert it to a long.
 * This function check that every characters are from the base
 * and that every character are used in the conversion.
 * This function does not set errno.
 * This function is a wrapper of the strToLongBase function.
 *
 * @param str The string to be parsed
 * @param isInBase A function that returns non zero if the character is from the base, 0 otherwise.
 * @param base The base of the string
 * @return The number parsed if there is no error.
 * If an error occured, it return long max.
 * @ref getLongMax
 */
long	stringToLongBase(const std::string& str, int (&isInBase)(int character), int base)
{
	const char *begin = str.c_str();
	const char *end = begin + str.size();

	return (strToLongBase(begin, end, isInBase, base));
}
