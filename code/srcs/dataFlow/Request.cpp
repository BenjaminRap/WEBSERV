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
	//Parsing the method
	const char	*meth = std::find(line, line + lineLength, ' ');
	if (*meth != ' ')
		return (1);
	this->_statusLine._method = std::string(line, meth - line);

	//Parsing the target
	const char	*targ = std::find(meth + 1, line + lineLength, ' ');
	if (*targ != ' ')
		return (1);
	this->_statusLine._requestTarget = std::string(meth + 1, targ - (meth + 1));

	//Parsing the protocol
	const char	*prot = std::find(targ + 1, line + lineLength, '\r');
	if (*prot != '\r' || *(prot + 1) != '\n')
		return (1);
	this->_statusLine._protocol = std::string(targ + 1, prot - (targ + 1));
	return (0);
}

int		Request::parseHeader(const char *line, size_t lineLength)
{
	const char *	pos;
	const char *	temp;

	pos = std::find(line, line + lineLength, ':');
	if (*pos == '\0' || *(pos + 1) != ' ')
		return (1);
	temp = std::find(line, line + lineLength, '\r');
	if (*temp == '\0' || *(temp + 1) != '\n')
		return (2);
	std::string key(line, pos - line);
	std::string value(pos + 2, temp - (pos + 2));
	this->_headers.insert(std::make_pair(key, value));
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
