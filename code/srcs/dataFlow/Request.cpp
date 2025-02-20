#include <iostream>

#include "Request.hpp"

void	Request::reset()
{
}

Body	*Request::getBody()
{
	return (_body);
}

int		Request::parseStatusLine(char *line, size_t lineLength)
{
	std::cout << "status line : " << std::string(line, lineLength) << std::endl;
	return (0);
}

int		Request::parseHeader(char *line, size_t lineLength)
{
	std::cout << "header :" << std::string(line, lineLength) << std::endl;
	return (0);
}