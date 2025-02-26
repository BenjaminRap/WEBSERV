#include "Request.hpp"

Request::Request(void)
{
	return ;
}

void	Request::reset()
{
	this->_statusLine._method.clear();
	this->_statusLine._protocol.clear();
	this->_statusLine._requestTarget.clear();
	this->_headers.clear();
}

Body	*Request::getBody() const
{
	return (_body);
}

int		Request::parseStatusLine(const char *line, size_t lineLength)
{
	std::string	s(line, lineLength);
	size_t		i = 0;
	size_t		pos;

	//Parsing the method
	pos = s.find(' ', i);
	if (pos == std::string::npos)
		return (1);
	this->_statusLine._method = s.substr(i, pos - i);
	i = pos + 1;

	//Parsing the target
	pos = s.find(' ', i);
	if (pos == std::string::npos)
		return (1);
	this->_statusLine._requestTarget = s.substr(i, pos - i);
	i = pos + 1;

	//Parsing the protocol
	pos = s.find("\r\n", i);
	if (pos == std::string::npos)
		return (1);
	this->_statusLine._protocol = s.substr(i, pos - i);
	return (0);
}

int		Request::parseHeader(const char *line, size_t lineLength)
{
	std::string	s(line, lineLength);
	size_t	i = 0;
	size_t	pos;
	size_t	temp;

	while (i < lineLength && !(s[i] == '\r' && s[i + 1] == '\n'))
	{
		pos = s.find(": ", i);
		if (pos == std::string::npos)
			return (1);
		temp = s.find("\r\n", pos);
		if (temp == std::string::npos)
			return (1);
		this->_headers.insert(std::make_pair(s.substr(i, pos - i), s.substr(pos + 2, temp - (pos + 2))));
		i = temp + 2;
	}
	return (0);
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
