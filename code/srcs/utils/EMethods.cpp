#include <cstring>		// for std::memcmp
#include <stdexcept>	// for std::logic_error

#include "EMethods.hpp"	// for EMethods

const std::string	methods[METHOD_COUNT] = { "GET", "POST", "DELETE", "PUT" };

const std::string&	getStringRepresentation(EMethods method)
{
	if (method < 0 || (int)method > METHOD_COUNT)
		throw std::logic_error("getStringRepresentation called with an invalid EMethod");
	return (methods[method]);
}

EMethods	getMethodFromBuffer(const char *str, size_t length)
{
	for (int i = 0; i < METHOD_COUNT; i++)
	{
		const std::string	method = methods[i];

		if (length != method.size())
			continue ;
		if (std::memcmp(str, method.c_str(), length) == 0)
			return ((EMethods)i);
	}
	throw std::invalid_argument("The string doesn't represent a valid method !");
}
