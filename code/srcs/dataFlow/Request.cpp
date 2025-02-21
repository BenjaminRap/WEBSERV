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

Request::Request(std::string line)
{
	size_t	i = 0;
	size_t	pos;
	size_t	end = line.find_first_of("\r\n\r\n", 0);
	size_t	temp;

	if (end == std::string::npos)
		throw (RequestException()"Wrong format!");

	//Parsing the method
	pos = line.find_first_of(' ', i);
	if (pos == std::string::npos || line[pos] != ' ')
		throw (RequestException("Wrong method!"));
	this->_statusLine._method = line.substr(i, pos - i);
	i = pos + 1;

	//Parsing the target
	pos = line.find_first_of(' ', i);
	if (pos == std::string::npos || line[pos] != ' ')
		throw (RequestException("Wrong request target!"));
	this->_statusLine._requestTarget = line.substr(i, pos - i);
	i = pos + 1;

	//Parsing the protocol
	pos = line.find_first_of("\r\n", i);
	if (pos == std::string::npos || line[pos] != '\r' || line[pos + 1] != '\n')
		throw (RequestException("Wrong protocol!"));
	this->_statusLine._protocol = line.substr(i, pos - i);
	i = pos + 2;

	//Parsing the header
	while (line[i] && i < end)
	{
		pos = line.find_first_of(": ", i);
		if (pos == std::string::npos || pos > end)
			throw (RequestException("Wrong header!"));
		temp = line.find_first_of(pos - i, "\r\n");
		this->_headers.insert(std::make_pair(line.substr(i, pos - i), line.substr(pos - i + 2, temp - (pos - i + 2))));
		i = temp + 2;
	}
}

Request::~Request(void)
{

}

std::ostream & operator<<(std::ostream & o, Request const & rhs)
{
	std::cout << "Method :" << rhs._statusLine._method << std::endl;
	std::cout << "Target :" << rhs._statusLine._requestTarget << std::endl;
	std::cout << "Protocol :" << rhs._statusLine._protocol << std::endl << std::endl;

	for (std::map<std::string ,std::string>::iterator it = rhs._headers.begin(); it != rhs.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
