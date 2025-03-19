#include <algorithm>      			// for find
#include <cstring>        			// for memcmp, NULL, size_t
#include <exception>      			// for exception
#include <iostream>       			// for basic_ostream, operator<<, endl, cout, basi...
#include <map>            			// for map, operator!=, _Rb_tree_const_iterator
#include <string>         			// for basic_string, char_traits, string, operator<<
#include <utility>        			// for pair, make_pair

#include "ABody.hpp"      			// for ABody
#include "EMethods.hpp"   			// for EMethods
#include "Request.hpp"    			// for Request, operator<<
#include "SizedBody.hpp"  			// for SizedBody
#include "requestStatusCode.hpp"	// for HTTP_...
#include "socketCommunication.hpp"	// for checkError

Request::Request(void) :
	_statusLine(),
	_headers(),
	_bodyDestFd(-1),
	_body(NULL)
{
}

Request::~Request(void)
{
	if (_body != NULL)
		delete _body;
	if (_bodyDestFd >= 0)
		checkError(close(_bodyDestFd), -1, "close() : ");
}

void	Request::reset()
{
	this->_statusLine.method = (EMethods)-1;
	this->_statusLine.requestTarget.clear();
	this->_headers.clear();
	if (_body != NULL)
	{
		delete _body;
		_body = NULL;
	}
	if (_bodyDestFd >= 0)
	{
		checkError(close(_bodyDestFd), -1, "close() : ");
		_bodyDestFd = -1;
	}
}

ABody	*Request::getBody() const
{
	return (_body);
}

int		Request::parseStatusLine(const char *line, size_t lineLength)
{
	//Parsing the method
	const char	*meth = std::find(line, line + lineLength, ' ');
	if (*meth != ' ')
		return (HTTP_BAD_REQUEST);
	if (!std::memcmp(line, "GET", 3))
		this->_statusLine.method = GET;
	else if (!std::memcmp(line, "POST", 4))
		this->_statusLine.method = POST;
	else if (!std::memcmp(line, "DELETE", 6))
		this->_statusLine.method = DELETE;
	else if (!std::memcmp(line, "PUT", 3))
		this->_statusLine.method = PUT;
	else
		return (HTTP_NOT_IMPLEMENTED);

	//Parsing the target
	const char	*targ = std::find(meth + 1, line + lineLength, ' ');
	if (*targ != ' ')
		return (HTTP_BAD_REQUEST);
	this->_statusLine.requestTarget = std::string(meth + 1, targ - (meth + 1));

	//Parsing the protocol
	const char	*prot = std::find(targ + 1, line + lineLength, '\r');
	if (*prot != '\r' || *(prot + 1) != '\n')
		return (HTTP_BAD_REQUEST);
	if (std::memcmp(targ + 1, "HTTP/1.1", 8))
		return (HTTP_HTTP_VERSION_NOT_SUPPORTED);
	return (HTTP_OK);
}

int		Request::parseHeader(const char *line, size_t lineLength)
{
	const char *	pos;
	const char *	temp;

	pos = std::find(line, line + lineLength, ':');
	if (*pos == '\0' || *(pos + 1) != ' ')
		return (HTTP_BAD_REQUEST);
	temp = std::find(line, line + lineLength, '\r');
	if (*temp == '\0' || *(temp + 1) != '\n')
		return (HTTP_BAD_REQUEST);
	std::string key(line, pos - line);
	std::string value(pos + 2, temp - (pos + 2));
	this->_headers.insert(std::make_pair(key, value));
	return (HTTP_OK);
}

EMethods	Request::getMethod(void) const
{
	return (this->_statusLine.method);
}

const std::string	&Request::getRequestTarget(void) const
{
	return (this->_statusLine.requestTarget);
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

bool	stringToSizeT(const  std::string &str, size_t &outValue);

int	Request::setBodyFromHeaders(int destFd, bool isBlocking)
{
	_bodyDestFd = destFd;
	const std::string * const	contentLengthString = getHeader("Content-Length");
	if (contentLengthString != NULL)
	{
		size_t contentLength = 0;
		if (stringToSizeT(*contentLengthString, contentLength) == false)
			return (HTTP_BAD_REQUEST);
		try
		{
			_body = new SizedBody(destFd, contentLength, isBlocking);
		}
		catch (const std::exception&)
		{
			return (HTTP_INTERNAL_SERVER_ERROR);
		}
		return (HTTP_OK);
	}
	return (HTTP_OK);
}

std::ostream & operator<<(std::ostream & o, Request const & rhs)
{
	const std::map<std::string, std::string>	&header = rhs.getHeaderMap();
	std::string	method[4] = {"GET", "POST", "DELETE", "PUT"};

	std::cout << "Method :" << method[rhs.getMethod()] << std::endl;
	std::cout << "Target :" << rhs.getRequestTarget() << std::endl;
	std::cout << "Protocol :" << "HTTP/1.1" << std::endl << std::endl;

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
