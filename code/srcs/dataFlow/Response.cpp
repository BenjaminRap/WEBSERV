#include "Response.hpp"
#include "ARequestType.hpp"

void	Response::setResponse(int code)
{
	this->_statusLine._statusCode = code;
	this->_statusLine._statusText = ARequestType::getStatusText(code);
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

	std::cout << rhs.getProtocol() << " " << rhs.getStatusCode() << " " << rhs.getStatusText() << std::endl;

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
