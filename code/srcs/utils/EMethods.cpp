#include "EMethods.hpp"

const std::string&	getStringRepresentation(EMethods method)
{
	const static std::string	methods[4] = { "GET", "POST", "DELETE", "PUT" };

	return (methods[method]);
}
