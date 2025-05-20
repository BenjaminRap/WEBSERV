#include <cctype>                   // for isdigit
#include <cstring>                  // for NULL, size_t
#include <iostream>                 // for operator<<, basic_ostream, ostream
#include <string>                   // for char_traits, basic_string, operat...

#include "AFdData.hpp"              // for AFdData
#include "ChunkedBody.hpp"          // for ChunkedBody
#include "EMethods.hpp"             // for EMethods, getStringRepresentation
#include "Headers.hpp"              // for Headers, operator<<
#include "Request.hpp"              // for Request, operator<<
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SharedResource.hpp"       // for SharedResource, freePointer
#include "SizedBody.hpp"            // for SizedBody
#include "protocol.hpp"             // for PROTOCOL
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_CONTENT_TO...

class ABody;  // lines 16-16

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

int	Request::setBodyFromHeaders(const ServerConfiguration& serverConf)
{
	const std::string * const	contentLengthString = _headers.getHeader("content-length");
	const std::string * const	transferEncoding = _headers.getHeader("transfer-encoding");
	const size_t				maxSize = serverConf.getMaxClientBodySize();

	if (contentLengthString != NULL && transferEncoding != NULL)
		return (HTTP_BAD_REQUEST);
	if (contentLengthString != NULL)
	{
		const unsigned long	contentLength = stringToULongBase(*contentLengthString, std::isdigit, 10);
		if (contentLength == (unsigned long)-1)
			return (HTTP_BAD_REQUEST);
		if ((size_t)contentLength > maxSize)
			return (HTTP_CONTENT_TOO_LARGE);
		_body.setManagedResource(new SizedBody(contentLength), freePointer);
	}
	else if (transferEncoding != NULL && *transferEncoding == "chunked")
		_body.setManagedResource(new ChunkedBody(maxSize), freePointer);
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


AFdData*	Request::getFdData()
{
	if (_fdData.isManagingValue() == false)
		return (NULL);
	return (_fdData.getValue());
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

void	Request::setFdData(const SharedResource<AFdData*>* fdData)
{
	if (fdData == NULL)
	{
		_fdData.stopManagingResource();
		return ;
	}
	if (!fdData->isManagingValue()
		|| !_body.isManagingValue())
	{
		return ;
	}
	_fdData = *fdData;

	const int fd =  _fdData.getValue()->getFd();
	_body.getValue()->setFd(fd);
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
