#include <stdexcept>

#include "EMethods.hpp"

const std::string&	getStringRepresentation(EMethods method)
{
	const static std::string	methods[4] = { "GET", "POST", "DELETE", "PUT" };

	if (method < 0 || method >= methods.size())
		throw std::logic_error("getStringRepresentation called with an invalid method");
	return (methods[method]);
}
