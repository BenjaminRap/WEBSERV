#include <cstring>                // for memcmp, size_t, NULL
#include <iostream>               // for basic_ostream, operator<<, endl, cout
#include <string>                 // for basic_string, char_traits, string

#include "CgiIn.hpp"			  // for CgiIn
#include "ChunkedBody.hpp"		  // for ChunkedBody
#include "EMethods.hpp"           // for EMethods, getStringRepresentation
#include "Request.hpp"            // for Request, operator<<
#include "ServerConfiguration.hpp"
#include "SharedResource.hpp"     // for SharedResource
#include "SizedBody.hpp"          // for SizedBody
#include "protocol.hpp"           // for PROTOCOL, PROTOCOL_LENGTH
#include "requestStatusCode.hpp"  // for HTTP_BAD_REQUEST, HTTP_OK, HTTP_HTT...

class ABody;

unsigned long	stringToULongBase(const std::string& str, int (&isInBase)(int character), int base);

/*****************************Constructors/Destructors*********************************/

Request::Request(void) :
	_statusLine(),
	_headers(),
	_fdData(),
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
	_fdData.stopManagingResource();
	_body.stopManagingResource();
}

int	Request::setBodyFromHeaders
(
	SharedResource<AFdData*> fdData,
	const ServerConfiguration& serverConfiguration
)
{
	_fdData = fdData;
	const std::string * const	contentLengthString = _headers.getHeader("content-length");
	const std::string * const	transferEncoding = _headers.getHeader("transfer-encoding");
	const size_t				maxSize = serverConfiguration.getMaxClientBodySize();
	const int 					fd = fdData.isManagingValue() ? fdData.getValue()->getFd() : -1;

	ABody*						body = NULL;

	if (contentLengthString != NULL && transferEncoding != NULL)
		return (HTTP_BAD_REQUEST);
	if (contentLengthString != NULL)
	{
		const unsigned long	contentLength = stringToULongBase(*contentLengthString, std::isdigit, 10);
		if (contentLength == (unsigned long)-1)
			return (HTTP_BAD_REQUEST);
		if ((size_t)contentLength > maxSize)
			return (HTTP_CONTENT_TOO_LARGE);
		body = new SizedBody(fd, contentLength);
	}
	else if (transferEncoding != NULL && *transferEncoding == "chunked")
		body = new ChunkedBody(fd, maxSize);
	else if (_statusLine.method == PUT || _statusLine.method == POST)
		return (HTTP_LENGTH_REQUIRED);
	_body.setManagedResource(body, freePointer);
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

Headers&	Request::getHeaders()
{
	return (_headers);
}

const Headers&	Request::getHeaders() const
{
	return (_headers);
}


bool	Request::isBodyBlocking() const
{
	if (_fdData.isManagingValue() == false)
		return (false);

	const AFdData * const	fdData = _fdData.getValue();
	return (fdData->getIsBlocking());
}

/******************************Operator Overload*****************************************/

std::ostream & operator<<(std::ostream & o, Request const & request)
{
	o << "Method:";
	if (request.getMethod() == (EMethods)-1)
		o << "unkown\n";
	else
		o << getStringRepresentation(request.getMethod()) << '\n';
	o << "Target :" << request.getRequestTarget() << '\n';
	o << "Protocol :" << PROTOCOL << "\n\n";

	o << request.getHeaders() << "\n";
	return (o);
}
