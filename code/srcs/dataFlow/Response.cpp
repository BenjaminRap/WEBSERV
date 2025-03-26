#include <stdint.h>         		// for uint16_t
#include <ctime>             		// for asctime, localtime, time, NULL, time_t
#include <iostream>          		// for basic_ostream, operator<<, basic_ios, cout
#include <iterator>          		// for reverse_iterator
#include <map>               		// for map, operator!=, _Rb_tree_const_iterator
#include <string>            		// for basic_string, char_traits, string, opera...
#include <unistd.h>					// for close
#include <utility>           		// for make_pair, pair

#include "ARequestType.hpp"  		// for ARequestType
#include "protocol.hpp"				// for PROTOCOL
#include "requestStatusCode.hpp"	// for HTTP_...
#include "Response.hpp"      		// for Response, operator<<
#include "SizedBody.hpp"			// for SizedBody

/*********************************Constructors/Destructors*************************************/

Response::Response(void) :
	_statusLine(),
	_headers(),
	_bodySrcFd(),
	_isBlocking(false),
	_body()
{
	reset();
}

Response::~Response(void)
{
}

/*********************************Privates Methods******************************************/

void	Response::addDefaultHeaders(void)
{
	const std::time_t	now = std::time(NULL);
	this->_headers.insert(std::make_pair("Date", std::asctime(std::localtime(&now))));
	this->_headers.rbegin()->second.erase(this->_headers.rbegin()->second.size() - 1, 1);
	this->_headers.insert(std::make_pair("Server", "WebServ de bg"));
	this->_headers.insert(std::make_pair("Connection", "keep-alive"));
}

std::string	sizeTToString(size_t value);

void	Response::setBody(ARequestType& requestResult, int socketFd)
{
	if (_bodySrcFd.isManagingValue() && _bodySrcFd.getValue() > 0)
	{
		const ssize_t	bodySize = requestResult.getOutSize();
		_body.setManagedResource(new SizedBody(socketFd, bodySize), freePointer);
		this->_headers.insert(std::make_pair("Content-Length", sizeTToString(bodySize)));
	}
	else
	this->_headers.insert(std::make_pair("Content-Length", "0"));
}

/*********************************Public Methods********************************************/

void	Response::setResponse(int code)
{
	this->_statusLine.statusCode = code;
	this->_statusLine.statusText = ARequestType::getStatusText(code);
	addDefaultHeaders();
	if (code >= 400)
	{
		_bodySrcFd.stopManagingResource();
	}
}

void	Response::setResponse(ARequestType& requestResult, int socketFd)
{
	_bodySrcFd = requestResult.getOutFd();
	setResponse(requestResult.getCode());
	if (requestResult.getRedirection().empty() == false)
		this->_headers.insert(std::make_pair("Location", requestResult.getRedirection()));
	setBody(requestResult, socketFd);
}

void	Response::reset()
{
	this->_statusLine.statusCode = HTTP_INTERNAL_SERVER_ERROR;
	this->_statusLine.statusText = ARequestType::getStatusText(HTTP_INTERNAL_SERVER_ERROR);
	this->_headers.clear();
	this->_bodySrcFd.stopManagingResource();
	this->_body.stopManagingResource();
}

/**********************************Getters**************************************************/

uint16_t	Response::getStatusCode(void) const
{
	return (this->_statusLine.statusCode);
}

const std::string	&Response::getStatusText(void) const
{
	return (this->_statusLine.statusText);
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

bool	Response::getIsBlocking(void) const
{
	return (_isBlocking);
}


SharedResource<int>	Response::getSrcBodyFd(void) const
{
	return (_bodySrcFd);
}

SharedResource<ABody*>	Response::getBody(void) const
{
	return (_body);
}

/*********************************Operator Overload**********************************************/

std::ostream & operator<<(std::ostream & o, Response const & rhs)
{
	const std::map<std::string, std::string>	&header = rhs.getHeaderMap();

	std::cout << PROTOCOL << " " << rhs.getStatusCode() << " " << rhs.getStatusText() << '\n';

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
