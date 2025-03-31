#include <algorithm>              // for find
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
	const std::string * const	contentLengthString = getHeader("Content-Length");
	if (contentLengthString != NULL)
	{
		size_t contentLength = 0;
		if (stringToSizeT(*contentLengthString, contentLength) == false)
			return (HTTP_BAD_REQUEST);
		if (contentLength > serverConfiguration.getMaxClientBodySize())
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
	return (HTTP_OK);
}

int		Request::parseStatusLine(const char *line, const char *end)
{
	//Parsing the method
	const char	*methodEnd = std::find(line, end, ' ');
	if (methodEnd == end)
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
	const char	*targetEnd = std::find(methodEnd + 1, end, ' ');
	if (targetEnd == end)
		return (HTTP_BAD_REQUEST);
	this->_statusLine.requestTarget = std::string(methodEnd + 1, targetEnd - 1);

	//Parsing the protocol
	const char	*protocolEnd = std::find(targetEnd + 1, end, '\r');
	if (protocolEnd == end)
		return (HTTP_BAD_REQUEST);
	if (std::memcmp(targetEnd + 1, PROTOCOL, PROTOCOL_LENGTH))
		return (HTTP_HTTP_VERSION_NOT_SUPPORTED);
	return (HTTP_OK);
}

int		Request::parseHeader(const char *line, const char *end)
{
	if (std::distance(line, end) < 5)
		return (HTTP_BAD_REQUEST);
	const char * const keyEnd = std::find(line, end, ':');

	if (keyEnd == end || *(keyEnd + 1) != ' ')
		return (HTTP_BAD_REQUEST);

	const char * const valueEnd = end - 1;
	if (*valueEnd != '\r')
		return (HTTP_BAD_REQUEST);
	const std::string key(line, keyEnd);
	const char * valuePosition = keyEnd + 2;
	const std::string value(valuePosition, valueEnd);
	this->_headers[key] = value;
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

	std::cout << "Method :" << getStringRepresentation(rhs.getMethod()) << std::endl;
	std::cout << "Target :" << rhs.getRequestTarget() << std::endl;
	std::cout << "Protocol :" << PROTOCOL << std::endl << std::endl;

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
