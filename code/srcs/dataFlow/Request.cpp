#include <cstring>                // for memcmp, size_t, NULL
#include <exception>              // for exception
#include <iostream>               // for basic_ostream, operator<<, endl, cout
#include <map>                    // for map, operator!=, _Rb_tree_const_ite...
#include <string>                 // for basic_string, char_traits, string
#include <utility>                // for pair, make_pair

#include "EMethods.hpp"           // for EMethods, getStringRepresentation
#include "Request.hpp"            // for Request, operator<<
#include "ServerConfiguration.hpp"
#include "SharedResource.hpp"     // for SharedResource
#include "SizedBody.hpp"          // for SizedBody
#include "protocol.hpp"           // for PROTOCOL, PROTOCOL_LENGTH
#include "requestStatusCode.hpp"  // for HTTP_BAD_REQUEST, HTTP_OK, HTTP_HTT...

class ABody;

long	getLongMax();
long	stringToLongBase(const std::string& str, int (&isInBase)(int character), int base);

/*****************************Constructors/Destructors*********************************/

Request::Request(void) :
	_statusLine(),
	_headers(),
	_bodyDestFd(),
	_isBlocking(false),
	_body()
{
	_statusLine.method = (EMethods)-1;
	_statusLine.requestTarget = "";
}

Request::~Request(void)
{
}

/*******************************Public Methods*********************************************/

void	Request::reset()
{
	this->_statusLine.method = (EMethods)-1;
	this->_statusLine.requestTarget.clear();
	this->_headers.clear();
	_bodyDestFd.stopManagingResource();
	_isBlocking = false;
	_body.stopManagingResource();
}

bool	stringToSizeT(const  std::string &str, size_t &outValue);

int	Request::setBodyFromHeaders(SharedResource<int> destFd, const ServerConfiguration& serverConfiguration)
{
	_bodyDestFd = destFd;
	const std::string * const	contentLengthString = getHeader("content-length");
	if (contentLengthString != NULL)
	{
		long	contentLength = stringToLongBase(*contentLengthString, std::isdigit, 10);
		if (contentLength == getLongMax())
			return (HTTP_BAD_REQUEST);
		if ((size_t)contentLength > serverConfiguration.getMaxClientBodySize())
			return (HTTP_CONTENT_TOO_LARGE);
		try
		{
			const int fd = _bodyDestFd.isManagingValue() ? _bodyDestFd.getValue() : -1;
			_body.setManagedResource(new SizedBody(fd, contentLength), freePointer);
		}
		catch (const std::exception&)
		{
			return (HTTP_INTERNAL_SERVER_ERROR);
		}
		return (HTTP_OK);
	}
	else if (_statusLine.method == PUT || _statusLine.method == POST)
		return (HTTP_LENGTH_REQUIRED);
	return (HTTP_OK);
}

/**********************************Getters*******************************************************/

ABody	*Request::getBody() const
{
	if (_body.isManagingValue() == false)
		return (NULL);
	return (_body.getValue());
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


bool	Request::getIsBlocking(void) const
{
	return (_isBlocking);
}

/******************************Operator Overload*****************************************/

std::ostream & operator<<(std::ostream & o, Request const & rhs)
{
	const std::map<std::string, std::string>	&header = rhs.getHeaderMap();

	o << "Method:";
	if (rhs.getMethod() == (EMethods)-1)
		o << "unkown\n";
	else
		o << getStringRepresentation(rhs.getMethod()) << '\n';
	o << "Target :" << rhs.getRequestTarget() << '\n';
	o << "Protocol :" << PROTOCOL << "\n\n";

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		o << it->first << ": " << it->second << '\n';
	}
	return (o);
}
