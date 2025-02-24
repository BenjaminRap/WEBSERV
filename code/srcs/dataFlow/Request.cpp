#include "Request.hpp"

void	Request::reset()
{
}

Body	*Request::getBody() const
{
	return (_body);
}

// int		Request::parseStatusLine(char *line, size_t lineLength)
// {
// 	std::cout << "status line : " << std::string(line, lineLength) << std::endl;
// 	return (0);
// }

// int		Request::parseHeader(char *line, size_t lineLength)
// {
// 	std::cout << "header :" << std::string(line, lineLength) << std::endl;
// 	return (0);
// }

Request::Request(std::string line)
{
	size_t	i = 0;
	size_t	pos;
	size_t	end = line.find("\r\n\r\n", 0);
	size_t	temp;

	if (end == std::string::npos)
		throw (RequestException("Wrong format!"));

	//Parsing the method
	pos = line.find(' ', i);
	if (pos == std::string::npos || line[pos] != ' ')
		throw (RequestException("Wrong method!"));
	this->_statusLine._method = line.substr(i, pos - i);
	i = pos + 1;

	//Parsing the target
	pos = line.find(' ', i);
	if (pos == std::string::npos || line[pos] != ' ')
		throw (RequestException("Wrong request target!"));
	this->_statusLine._requestTarget = line.substr(i, pos - i);
	i = pos + 1;

	//Parsing the protocol
	pos = line.find("\r\n", i);
	if (pos == std::string::npos || line[pos] != '\r' || line[pos + 1] != '\n')
		throw (RequestException("Wrong protocol!"));
	this->_statusLine._protocol = line.substr(i, pos - i);
	i = pos + 2;

	//Parsing the header
	while (line[i] && i < end)
	{
		pos = line.find(": ", i);
		if (pos == std::string::npos || pos > end)
			throw (RequestException("Wrong header!"));
		temp = line.find("\r\n", pos);
		if (temp == std::string::npos)
			break;
		this->_headers.insert(std::make_pair(line.substr(i, pos - i), line.substr(pos + 2, temp - (pos + 2))));
		i = temp + 2;
	}
}

Request::~Request(void)
{

}

const std::string	&Request::getMethod(void) const
{
	return (this->_statusLine._method);
}

const std::string	&Request::getRequestTarget(void) const
{
	return (this->_statusLine._requestTarget);
}

const std::string	&Request::getProtocol(void) const
{
	return (this->_statusLine._protocol);
}

const std::string	*Request::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);

	if (it != this->_headers.end())
		return (&it->second);
	return (NULL);
}

const std::map<std::string, std::string>	&Request::getHeaderMap(void) const
{
	return (this->_headers);
}

std::ostream & operator<<(std::ostream & o, Request const & rhs)
{
	const std::map<std::string, std::string>	&header = rhs.getHeaderMap();

	std::cout << "Method :" << rhs.getMethod() << std::endl;
	std::cout << "Target :" << rhs.getRequestTarget() << std::endl;
	std::cout << "Protocol :" << rhs.getProtocol() << std::endl << std::endl;

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
