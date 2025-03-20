#include <cerrno>
#include <cstdlib>
#include <string>
#include <climits>
#include <sstream>

bool	stringToSizeT(const  std::string &str, size_t &outValue)
{
	char	*endPtr;
	outValue = std::strtoul(str.c_str(), &endPtr, 10);
	if (endPtr == str.c_str() || *endPtr != '\0')
		return (false);
	if (outValue == ULONG_MAX && errno == ERANGE)
		return (false);
	return (true);
}

std::string	sizeTToString(size_t value)
{
	std::ostringstream	strStream;

	strStream << value;
	return (strStream.str());
}
