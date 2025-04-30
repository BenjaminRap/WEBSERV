#include <errno.h>                  // for errno, EACCES, ENOENT
#include <fcntl.h>                  // for open, O_RDONLY
#include <stdint.h>                 // for uint16_t
#include <ctime>                    // for NULL, gmtime, strftime, time, size_t
#include <iostream>                 // for basic_ostream, operator<<, cout
#include <map>                      // for map, operator!=, _Rb_tree_const_i...
#include <string>                   // for basic_string, char_traits, string
#include <utility>                  // for make_pair, pair

#include "ARequestType.hpp"         // for ARequestType
#include "Response.hpp"             // for Response, operator<<
#include "FileFd.hpp"				// for FileFd
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SharedResource.hpp"       // for SharedResource
#include "SizedBody.hpp"            // for SizedBody
#include "Status.hpp"               // for Status, StatusType
#include "exception.hpp"            // for CustomException
#include "protocol.hpp"             // for PROTOCOL
#include "requestStatusCode.hpp"    // for HTTP_FORBIDDEN, HTTP_INTERNAL_SER...

class ABody;

/*********************************Constructors/Destructors*************************************/

Response::Response(const ServerConfiguration& defaultConfig) :
	_status(NULL),
	_headers(),
	_fdData(),
	_body(),
	_defaultConfig(defaultConfig),
	_autoIndexPage()
{
	reset();
}

Response::~Response(void)
{
}

/*********************************Privates Methods******************************************/

void	Response::addDefaultHeaders(void)
{
	char				timeBuffer[100];
	const std::time_t	now = std::time(NULL);

	std::strftime(timeBuffer, 100, "%c", std::gmtime(&now));
	_headers["date"] = timeBuffer;
	_headers["server"] = "WebServ de bg";
	_headers["connection"] = (_status->isOfType(STATUS_ERROR) ? "close" : "keep-alive");
}

std::string	sizeTToString(size_t value);

void	Response::setBody(ARequestType* requestResult, int socketFd)
{
	size_t	bodySize = 0;

	if (requestResult != NULL && _fdData.isManagingValue())
	{
		bodySize = requestResult->getOutSize();
		_body.setManagedResource(new SizedBody(socketFd, bodySize), freePointer);
	}
	else if (_status->getErrorPage().size() != 0)
		bodySize = _status->getErrorPage().size();
	else
		bodySize = _autoIndexPage.size();
	this->_headers.insert(std::make_pair("content-length", sizeTToString(bodySize)));
}


uint16_t	getStatusCodeFromErrno(int errnoValue)
{
	switch (errnoValue)
	{
	case ENOENT:
		return (HTTP_NOT_FOUND);
	case EACCES:
		return (HTTP_FORBIDDEN);
	default:
		return (HTTP_INTERNAL_SERVER_ERROR);
	}
}

uint16_t	Response::setErrorPage(uint16_t code, const ServerConfiguration& serverConfiguration)
{
	if (Status::isCodeOfType(code, STATUS_ERROR) == false)
		return (code);
	_fdData.stopManagingResource();
	try
	{
		const std::string* errorPage = serverConfiguration.getErrorPage(code);
		if (errorPage == NULL)
			return (code);

		FileFd*				fileFd = new FileFd(*errorPage, O_RDONLY);
		_fdData.setManagedResource(fileFd, freePointer);
	}
	catch (const FileFd::FileOpeningError& openError)
	{
		return (getStatusCodeFromErrno(openError.getErrno()));
	}
	return (code);
}

void	Response::initValues(int code, const ServerConfiguration& serverConfiguration, ARequestType *requestResult, int socketFd)
{
	code = setErrorPage(code, serverConfiguration); // the order is important because it changes the code
	_status = Status::getStatus(code);
	if (_status == NULL)
		throw std::logic_error("Response setResponse called with an invalid status code !");
	addDefaultHeaders();
	setBody(requestResult, socketFd);
}

/*********************************Public Methods********************************************/

void	Response::setResponse(int code)
{
	reset();
	initValues(code, _defaultConfig, NULL, -1);
}

void	Response::setResponse(ARequestType& requestResult, int socketFd)
{
	reset();
	_fdData = requestResult.getOutFd();
	_autoIndexPage = requestResult.getAutoIndexPage();
	initValues(requestResult.getCode(), requestResult.getConfig(), &requestResult, socketFd);
	if (requestResult.getRedirection().empty() == false
		&& _status->isOfType(STATUS_REDIRECTION))
	{
		this->_headers.insert(std::make_pair("Location", requestResult.getRedirection()));
	}
}

void	Response::reset()
{
	this->_status = NULL;
	this->_headers.clear();
	this->_fdData.stopManagingResource();
	this->_body.stopManagingResource();
	this->_autoIndexPage = "";
}

/**********************************Getters**************************************************/

const Headers&	Response::getHeaders(void) const
{
	return (this->_headers);
}

Headers&	Response::getHeaders(void)
{
	return (this->_headers);
}

SharedResource<AFdData*>	Response::getFdData(void) const
{
	return (_fdData);
}

SharedResource<ABody*>	Response::getBody(void) const
{
	return (_body);
}

const Status*	Response::getStatus(void) const
{
	return (_status);
}

const std::string&	Response::getAutoIndexPage(void) const
{
	return (_autoIndexPage);
}

/*********************************Operator Overload**********************************************/

std::ostream & operator<<(std::ostream & o, Response const & response)
{
	const Status * const						status = response.getStatus();

	if (status == NULL)
		o << "unset unset unset\n";
	else
		o << status->getRepresentation();

	o << response.getHeaders();
	if (status != NULL)
	{
		if (status->isOfType(STATUS_ERROR))
			o << status->getErrorPage();
		else if (status->isOfType(STATUS_SUCESSFULL))
			o << response.getAutoIndexPage();
	}
	return (o);
}
