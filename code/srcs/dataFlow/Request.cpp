#include <iostream>

#include "Request.hpp"

void	Request::reset()
{
}

Body	*Request::getBody()
{
	return (_body);
}

int		Request::parseStatusLine(char *start, char *last)
{
	std::cout << "status line : " << std::string(start, last) << std::endl;
	return (0);
}

int		Request::parseHeader(char *start, char *last)
{
	std::cout << "header :" << std::string(start, last) << std::endl;
	return (0);
}