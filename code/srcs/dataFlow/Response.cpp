#include <stdint.h>         		// for uint16_t
#include <ctime>             		// for asctime, localtime, time, NULL, time_t
#include <iostream>          		// for basic_ostream, operator<<, basic_ios, cout
#include <iterator>          		// for reverse_iterator
#include <map>               		// for map, operator!=, _Rb_tree_const_iterator
#include <string>            		// for basic_string, char_traits, string, opera...
#include <utility>           		// for make_pair, pair

#include "ARequestType.hpp"  		// for ARequestType
#include "requestStatusCode.hpp"	// for HTTP_...
#include "socketCommunication.hpp"	// for checkError
#include "Response.hpp"      		// for Response, operator<<


Response::Response(void) :
	_statusLine(),
	_headers(),
	_bodySrcFd(-1),
	_body(NULL)
{
	reset();
}

Response::~Response(void)
{
	if (_bodySrcFd != -1)
		checkError(close(_bodySrcFd), -1, "close() : ");
	if (_body != NULL)
		delete _body;
}

void	Response::setResponse(int code, const std::string &redirection)
{
	this->_statusLine.statusCode = code;
	this->_statusLine.statusText = ARequestType::getStatusText(code);
	this->_statusLine.protocol = "HTTP/1.1";
	const std::time_t	now = std::time(NULL);
	this->_headers.insert(std::make_pair("Date", std::asctime(std::localtime(&now))));
	this->_headers.rbegin()->second.erase(this->_headers.rbegin()->second.size() - 1, 1);
	this->_headers.insert(std::make_pair("Server", "WebServ de bg"));
	this->_headers.insert(std::make_pair("Connection", "keep-alive"));
	if (redirection.empty() == false)
		this->_headers.insert(std::make_pair("Location", redirection));
}

void	Response::reset()
{
	this->_statusLine.statusCode = HTTP_INTERNAL_SERVER_ERROR;
	this->_statusLine.statusText = ARequestType::getStatusText(HTTP_INTERNAL_SERVER_ERROR);
	this->_headers.clear();
	this->_bodySrcFd = -1;
	this->_body = NULL;
}

uint16_t	Response::getStatusCode(void) const
{
	return (this->_statusLine.statusCode);
}

const std::string	&Response::getStatusText(void) const
{
	return (this->_statusLine.statusText);
}

const std::string	&Response::getProtocol(void) const
{
	return (this->_statusLine.protocol);
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
