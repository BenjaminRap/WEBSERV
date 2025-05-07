#include <errno.h>                  // for EACCES, ENOENT
#include <fcntl.h>                  // for O_RDONLY
#include <stdint.h>                 // for uint16_t
#include <ctime>                    // for NULL, gmtime, strftime, time, size_t
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, operator<<, ostream
#include <map>                      // for map
#include <stdexcept>                // for logic_error
#include <string>                   // for basic_string, operator<<, string
#include <utility>                  // for make_pair

#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "ARequestType.hpp"         // for ARequestType
#include "FileFd.hpp"               // for FileFd
#include "Headers.hpp"              // for Headers, operator<<
#include "Response.hpp"             // for Response, operator<<
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SharedResource.hpp"       // for SharedResource, freePointer
#include "Status.hpp"               // for Status, StatusType
#include "requestStatusCode.hpp"    // for HTTP_INTERNAL_SERVER_ERROR, HTTP_...

/*********************************Constructors/Destructors*************************************/

Response::Response(const ServerConfiguration& defaultConfig) :
	_status(NULL),
	_headers(),
	_fdData(),
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

void	Response::setBody()
{
	size_t	bodySize = 0;

	if (_fdData.isManagingValue())
	{
		AFdData*	fdData = _fdData.getValue();

		if (fdData->getType() == FILE_FD)
			bodySize = static_cast<FileFd*>(fdData)->getSize();
		else
		{
			_fdData.stopManagingResource();
			throw std::logic_error("The AFdData is not a FileFd !");
		}
	}
	else if (_status->getErrorPage().size() != 0)
		bodySize = _status->getErrorPage().size();
	else
		bodySize = _autoIndexPage.size();
	this->_headers.insert(std::make_pair("content-length", sizeTToString(bodySize)));
}


/**
 * @brief get the HTTP status code corresponding to the value of errno
 * set when opening a file fail.
 *
 * @param errnoValue The value of errno set after an opening failed
 * @return a HTTP status code.
 */
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

/**
 * @brief Return a FileFd opened on the error page corresponding to
 * the status error. If an error occured, it can change the currentStatus.
 * If there is no error page corresponding to the status, returns NULL.
 *
 */
FileFd*	getErrorPage(const Status** currentStatus, const ServerConfiguration& serverConfiguration)
{
	try
	{
		const std::string* errorPage = serverConfiguration.getErrorPage((*currentStatus)->getCode());

		if (errorPage == NULL)
			return (NULL);

		return (new FileFd(*errorPage, O_RDONLY));
	}
	catch (const FileFd::FileOpeningError& openError)
	{
		const uint16_t	code = getStatusCodeFromErrno(openError.getErrno());

		*currentStatus = Status::getStatus(code);
	}
	catch (const std::exception& exception)
	{
		*currentStatus = Status::getStatus(HTTP_INTERNAL_SERVER_ERROR);
	}
			return (NULL);
}

void	Response::setErrorPage(const ServerConfiguration& serverConfiguration)
{
	_fdData.stopManagingResource();
	FileFd * errorPage = getErrorPage(&_status, serverConfiguration);
	if (errorPage != NULL)
		_fdData.setManagedResource(errorPage, freePointer);
}

void	Response::initValues(int code, const ServerConfiguration& serverConfiguration)
{
	_status = Status::getStatus(code);
	if (_status == NULL)
		throw std::logic_error("initValues called with an invalid code !");

	if (Status::isCodeOfType(code, STATUS_ERROR))
		setErrorPage(serverConfiguration);
	addDefaultHeaders(_headers, _status);
	setBody();
}

/*********************************Public Methods********************************************/

void	Response::setResponse(uint16_t code)
{
	reset();
	initValues(code, _defaultConfig);
}

void	Response::setResponse(ARequestType& requestResult)
{
	reset();

	_fdData = requestResult.getOutFd();
	if (_fdData.isManagingValue())
	{
		AFdData* fdData = _fdData.getValue();

		if (fdData->getType() == CGI_OUT)
			return ;
	}

	_autoIndexPage = requestResult.getAutoIndexPage();
	initValues(requestResult.getCode(), requestResult.getConfig());
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
