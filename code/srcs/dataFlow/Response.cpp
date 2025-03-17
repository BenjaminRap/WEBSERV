#include "Response.hpp"

void	Response::setResponseCode(int code, std::string text)
{
	this->_statusLine._statusCode = code;
	this->_statusLine._statusText = text;
	this->_statusLine._protocol = "HTTP/1.1";
	const std::time_t	now = std::time(NULL);
	this->_headers.insert(std::make_pair("Date", std::asctime(std::localtime(&now))));
	this->_headers.rbegin()->second.erase(this->_headers.rbegin()->second.size() - 1, 1);
	this->_headers.insert(std::make_pair("Server", "WebServ de bg"));
	this->_headers.insert(std::make_pair("Connection", "keep-alive"));
}

void	Response::reset()
{
	this->_statusLine._statusCode = 0;
	this->_statusLine._statusText.clear();
	this->_statusLine._protocol.clear();
	this->_headers.clear();
	this->_bodyFd = -1;
}

int			Response::getBodyFd(void) const
{
	return (this->_bodyFd);
}
char		*Response::getRawData(void) const
{
	std::stringstream s;
	s << *this;
	std::string header = s.str();
	char *raw_data = new char[header.size() + 1];
	std::memcpy(raw_data, header.c_str(), header.size() + 1);
	std::cout << raw_data << std::endl;
	return (raw_data);
}

uint16_t	Response::getStatusCode(void) const
{
	return (this->_statusLine._statusCode);
}

const std::string	&Response::getStatusText(void) const
{
	return (this->_statusLine._statusText);
}

const std::string	&Response::getProtocol(void) const
{
	return (this->_statusLine._protocol);
}

const std::string	*Response::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = this->_headers.find(key);

	if (it != this->_headers.end())
		return (&it->second);
	return (NULL);
}

const std::map<std::string, std::string>	&Response::getHeaderMap(void) const
{
	return (this->_headers);
}

std::ostream & operator<<(std::ostream & o, Response const & rhs)
{
	const std::map<std::string, std::string>	&header = rhs.getHeaderMap();

	o << rhs.getProtocol() << " " << rhs.getStatusCode() << " " << rhs.getStatusText() << "\r\n";

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		o << it->first << ": " << it->second << "\r\n";
	}
	o << "\r\n";
	return (o);
}
