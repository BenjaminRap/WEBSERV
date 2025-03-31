#include "EMethods.hpp"

const std::string&	getStringRepresentation(EMethods method)
{
	const static std::string	methods[METHOD_COUNT] = { "GET", "POST", "DELETE", "PUT" };

	return (methods[method]);
}
