#include <stddef.h>                 // for NULL
#include <unistd.h>                 // for close
#include <map>                      // for map, operator==
#include <stdexcept>                // for logic_error
#include <string>                   // for string, basic_string
#include <utility>                  // for pair

#include "ARequestType.hpp"         // for ARequestType
#include "EMethods.hpp"             // for EMethods
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_METHOD_NOT...

class Route;

bool	checkAllowMeth(const Route &root, EMethods meth);
void	delString(const std::string &toDel, std::string &str);
void	buildNewURl(std::string root, std::string &url);
void	replaceUrl(const std::string &location, const std::string &root, std::string &url);
void	fixPath(std::string &path);
void	fixUrl(ARequestType &req, std::string &url);
void	addRoot(ARequestType &get, const ServerConfiguration &config);

ARequestType::ARequestType(std::string &url, const ServerConfiguration& config, EMethods method) :
	_method(method),
	_config(config),
	_route(NULL),
	_url(url),
	_code(0),
	_redirection(""),
	_inFd(),
	_outFd(),
	_outSize(0)
{
	fixUrl(*this, url);
	if (getCode() == HTTP_BAD_REQUEST)
		return ;
	addRoot(*this, config);
	if (this->_code == HTTP_MOVED_PERMANENTLY || this->_code == HTTP_METHOD_NOT_ALLOWED)
		return;
	if (this->_url[0] != '.')
		this->_url.insert(0, ".");
}

ARequestType::~ARequestType()
{
}

/**
 * @brief Returns the status text corresponding to the current code.
 * @throw If the current code doesn't correspond to a status text, throw a std::logic_error
 * @note This function should only be called if the request is done.
 */
const std::string	&ARequestType::getStatusText(int code)
{
	const std::map<int, std::string>::const_iterator	it = ARequestType::_statuses.find(code);
	if (it == _statuses.end())
		throw std::logic_error("Unkown status code !");
	return (it->second);
}

void	ARequestType::setRedirectionResponse(int code, const std::string &redirection)
{
	this->_code = code;
	this->_redirection = redirection;
}


void	ARequestType::setResponse(int code)
{
	this->_code = code;
}

void	ARequestType::setUrl(const std::string &src)
{
	this->_url = src;
}

void	ARequestType::setRoute(const Route *route)
{
	this->_route = route;
}

void	ARequestType::setMethod(EMethods method)
{
	this->_method = method;
}

std::string	&ARequestType::getUrl()
{
	return (this->_url);
}

const std::string	&ARequestType::getRedirection() const
{
	return (this->_redirection);
}

const Route	*ARequestType::getRoute() const
{
	return (this->_route);
}

int	ARequestType::getCode() const
{
	return (this->_code);
}

EMethods	ARequestType::getMethod() const
{
	return (this->_method);
}

SharedResource<int>	ARequestType::getInFd()
{
	return (_inFd);
}

SharedResource<int>	ARequestType::getOutFd()
{
	return (_outFd);
}

size_t	ARequestType::getOutSize() const
{
	return (_outSize);
}

bool	ARequestType::getAutoIndex(void) const
{
	if (_route == NULL)
		return (false);
	return (_route->getAutoIndex());
}

const std::vector<std::string>&	ARequestType::getIndexs(void) const
{
	if (_route == NULL)
		return (_config.getIndex());
	return (_route->getIndex());
}

const std::map<uint16_t, std::string>&	ARequestType::getErrorPages(void) const
{
	// for an upgrade : do the same as getIndexs.
	return (_config.getErrorPages());
}

const ServerConfiguration&	ARequestType::getConfig() const
{
	return (_config);
}
