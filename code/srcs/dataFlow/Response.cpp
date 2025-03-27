#include <fcntl.h>					// for O_RDONLY
#include <stdint.h>         		// for uint16_t
#include <ctime>             		// for asctime, localtime, time, NULL, time_t
#include <iostream>          		// for basic_ostream, operator<<, basic_ios, cout
#include <iterator>          		// for reverse_iterator
#include <map>               		// for map, operator!=, _Rb_tree_const_iterator
#include <string>            		// for basic_string, char_traits, string, opera...
#include <unistd.h>					// for close
#include <utility>           		// for make_pair, pair

#include "ARequestType.hpp"  		// for ARequestType
#include "ServerConfiguration.hpp"
#include "Status.hpp"				// for Status::isError, Status::getStatus
#include "exception.hpp"			// for CustomException
#include "protocol.hpp"				// for PROTOCOL
#include "requestStatusCode.hpp"	// for HTTP_...
#include "Response.hpp"      		// for Response, operator<<
#include "SizedBody.hpp"			// for SizedBody
#include "socketCommunication.hpp"	// for closeFdAndPrintError

/*********************************Constructors/Destructors*************************************/

Response::Response(const ServerConfiguration& defaultConfig) :
	_status(NULL),
	_headers(),
	_bodySrcFd(),
	_isBlocking(false),
	_body(),
	_defaultConfig(defaultConfig)
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
	if (_status->isOfType(STATUS_ERROR))
		this->_headers.insert(std::make_pair("Connection", "close"));
	else
		this->_headers.insert(std::make_pair("Connection", "keep-alive"));
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
		bodySize = 0;
	this->_headers.insert(std::make_pair("Content-Length", sizeTToString(bodySize)));
}

/*
* @brief Set the error page if there is an error. It can change the status code and status text
* if there is an error finding the error page.
* @note it should be called before the functions adding headers as it can change the code.
*/
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
	initValues(code, _defaultConfig, NULL, -1);
}

void	Response::setResponse(ARequestType& requestResult, int socketFd)
{
	_bodySrcFd = requestResult.getOutFd();
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
	this->_body.stopManagingResource();
}

/**********************************Getters**************************************************/

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

const Status*	Response::getStatus(void) const
{
	return (_status);
}

/*********************************Operator Overload**********************************************/

std::ostream & operator<<(std::ostream & o, Response const & rhs)
{
	const std::map<std::string, std::string>&	header = rhs.getHeaderMap();
	const Status * const						status = rhs.getStatus();

	std::cout << PROTOCOL << " ";
	if (status == NULL)
		std::cout << "unset unset\n";
	else
		std::cout << status->getCode() << " " << status->getText() << '\n';

	for (std::map<std::string ,std::string>::const_iterator it = header.begin(); it != header.end(); ++it)
	{
		std::cout << it->first << ": " << it->second << std::endl;
	}
	return (o);
}
