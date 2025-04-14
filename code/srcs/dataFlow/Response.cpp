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
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SharedResource.hpp"       // for SharedResource
#include "SizedBody.hpp"            // for SizedBody
#include "Status.hpp"               // for Status, StatusType
#include "exception.hpp"            // for CustomException
#include "protocol.hpp"             // for PROTOCOL
#include "requestStatusCode.hpp"    // for HTTP_FORBIDDEN, HTTP_INTERNAL_SER...
#include "socketCommunication.hpp"  // for closeFdAndPrintError

class ABody;

/*********************************Constructors/Destructors*************************************/

Response::Response(const ServerConfiguration& defaultConfig) :
	_status(NULL),
	_headers(),
	_bodySrcFd(),
	_isBlocking(false),
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

	if (requestResult != NULL && _bodySrcFd.isManagingValue() && _bodySrcFd.getValue() > 0)
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

uint16_t	Response::setErrorPage(uint16_t code, const ServerConfiguration& serverConfiguration)
{
	if (Status::isCodeOfType(code, STATUS_ERROR) == false)
		return (code);
	_bodySrcFd.stopManagingResource();
	try
	{
		const std::string& errorPage = serverConfiguration.getErrorPage(code);
		const int fd = open(errorPage.c_str(), O_RDONLY);
		if (fd != -1)
		{
			_bodySrcFd.setManagedResource(fd, closeFdAndPrintError);
			return (code);
		}
		if (errno == ENOENT)
			return (HTTP_NOT_FOUND);
		else if (errno == EACCES)
			return (HTTP_FORBIDDEN);
		else
			return (HTTP_INTERNAL_SERVER_ERROR);
	}
	catch (const CustomException& exception)
	{
		return (code);
	}
}

void	Response::initValues(int code, const ServerConfiguration& serverConfiguration, ARequestType *requestResult, int socketFd)
{
	code = setErrorPage(code, serverConfiguration); // the order is important because it changes the code
	_status = &Status::getStatus(code);
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
	_bodySrcFd = requestResult.getOutFd();
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
	this->_bodySrcFd.stopManagingResource();
	this->_isBlocking = false;
	this->_body.stopManagingResource();
	this->_autoIndexPage = "";
}

/**********************************Getters**************************************************/

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

const Status*	Response::getStatus(void) const
{
	return (_status);
}

const std::string&	Response::getAutoIndexPage(void) const
{
	return (_autoIndexPage);
}

/*********************************Operator Overload**********************************************/

std::ostream & operator<<(std::ostream & o, Response const & rhs)
{
	const std::map<std::string, std::string>&	header = rhs.getHeaderMap();
	const Status * const						status = rhs.getStatus();

	o << PROTOCOL << " ";
	if (status == NULL)
		o << "unset unset\n";
	else
		o << status->getCode() << " " << status->getText() << '\n';

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		o << it->first << ": " << it->second << '\n';
	}
	if (status != NULL)
	{
		if (status->isOfType(STATUS_ERROR))
			o << status->getErrorPage();
		else if (status->isOfType(STATUS_SUCESSFULL))
			o << rhs.getAutoIndexPage();
	}
	return (o);
}
