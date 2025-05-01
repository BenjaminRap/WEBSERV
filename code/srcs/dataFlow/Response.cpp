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

void	addDefaultHeaders(Headers& headers, const Status* status)
{
	char				timeBuffer[100];
	const std::time_t	now = std::time(NULL);

	std::strftime(timeBuffer, 100, "%c", std::gmtime(&now));
	headers["date"] = timeBuffer;
	headers["server"] = "WebServ de bg";
	headers["connection"] = (status->isOfType(STATUS_ERROR) ? "close" : "keep-alive");

}

std::string	sizeTToString(size_t value);

void	Response::setBody(int socketFd)
{
	size_t	bodySize = 0;

	if (_fdData.isManagingValue())
	{
		AFdData*	fdData = _fdData.getValue();

		if (fdData->getType() == FILEFD)
			bodySize = static_cast<FileFd*>(fdData)->getSize();
		else
		{
			_fdData.stopManagingResource();
			throw std::logic_error("The AFdData is not a FileFd !");
		}
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

const Status*	Response::setErrorPage(const Status* currentStatus, const ServerConfiguration& serverConfiguration)
{
	_fdData.stopManagingResource();
	try
	{
		const std::string* errorPage = serverConfiguration.getErrorPage(currentStatus->getCode());
		if (errorPage == NULL)
			return (currentStatus);

		FileFd*				fileFd = new FileFd(*errorPage, O_RDONLY);
		_fdData.setManagedResource(fileFd, freePointer);
	}
	catch (const FileFd::FileOpeningError& openError)
	{
		const uint16_t	code = getStatusCodeFromErrno(openError.getErrno());

		return (Status::getStatus(code));
	}
	return (currentStatus);
}

void	Response::initValues(int code, const ServerConfiguration& serverConfiguration, int socketFd)
{
	_status = Status::getStatus(code);
	if (_status == NULL)
		throw std::logic_error("initValues called with an invalid code !");

	if (Status::isCodeOfType(code, STATUS_ERROR))
		_status = setErrorPage(_status, serverConfiguration);
	addDefaultHeaders(_headers, _status);
	setBody(socketFd);
}

/*********************************Public Methods********************************************/

void	Response::setResponse(int code)
{
	reset();
	initValues(code, _defaultConfig, -1);
}

void	Response::setResponse(ARequestType& requestResult, int socketFd)
{
	reset();

	_fdData = requestResult.getOutFd();
	if (_fdData.isManagingValue())
	{
		AFdData* fdData = _fdData.getValue();

		if (fdData->getType() == CGIOUT)
			return ;
	}

	_autoIndexPage = requestResult.getAutoIndexPage();
	initValues(requestResult.getCode(), requestResult.getConfig(), socketFd);
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
