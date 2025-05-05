#include <cerrno>   // for errno, ERANGE
#include <cstdlib>  // for strtoul, size_t
#include <sstream>  // for basic_ostringstream, basic_ostream, ostringstream
#include <string>   // for allocator, basic_string, string, char_traits

std::string	sizeTToString(size_t value)
{
	std::ostringstream	strStream;

	strStream << value;
	return (strStream.str());
}

/**
 * @brief Parse the string from begin to end, and convert it to an unsigned long.
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
 * If an error occured, it return unsigned long max.
 */
unsigned long	strToULongBase(const char *begin, const char* end, int (&isInBase)(int character), int base)
{
	for (const char* index = begin; index != end; index++)
	{
		if (!isInBase(*index))
			return (-1);
	}
	const int	errnoSave = errno;

	char*		numberEnd;

	errno = 0;
	const long	number = std::strtoul(begin, &numberEnd, base);
	const bool	numberTooBig = errno == ERANGE;
	errno = errnoSave;
	if (begin == numberEnd || numberTooBig || numberEnd != end)
		return (-1);
	return (number);
}

/**
 * @brief Parse the string and convert it to an unsigned long.
 * This function check that every characters are from the base
 * and that every character are used in the conversion.
 * This function does not set errno.
 * This function is a wrapper of the strToLongBase function.
 *
 * @param str The string to be parsed
 * @param isInBase A function that returns non zero if the character is from the base, 0 otherwise.
 * @param base The base of the string
 * @return The number parsed if there is no error.
 * If an error occured, it return unsigned long max.
 */
unsigned long	stringToULongBase(const std::string& str, int (&isInBase)(int character), int base)
{
	const char *begin = str.c_str();
	const char *end = begin + str.size();

	return (strToULongBase(begin, end, isInBase, base));
}
