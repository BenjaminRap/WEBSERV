#include <stdint.h>                 // for uint16_t
#include <cstring>                  // for NULL, size_t
#include <list>                     // for list
#include <map>                      // for map
#include <string>                   // for string, basic_string, operator==
#include <vector>                   // for vector

#include "ARequestType.hpp"         // for ARequestType
#include "EMethods.hpp"             // for EMethods
#include "Request.hpp"              // for Request
#include "RequestContext.hpp"       // for RequestContext
#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "Status.hpp"               // for Status, StatusType
#include "parsing.hpp"              // for ConfigHeaders
#include "requestStatusCode.hpp"    // for HTTP_NOT_FOUND

class AFdData;  // lines 21-21
template <typename T> class SharedResource;

bool		checkAllowMeth(ARequestType& req, EMethods meth);
bool		fixUrl(ARequestType &req, std::string &url);
void		addRoot(ARequestType &req, const ServerConfiguration &config);
void		extractQueryString(std::string& url, std::string& queryString);
uint16_t	isCgiExecutable(const std::string& path, uint16_t targetType);
bool		setRedirection(ARequestType& req);
uint16_t	isDirOrFile(const std::string& path);
bool		isExtension(const std::string& file, const std::string& extension);


ARequestType::ARequestType
(
	std::string &url,
	const ServerConfiguration& config,
	EMethods method,
	const std::string& domain,
	RequestContext& requestContext
) :
	_config(config),
	_route(NULL),
	_path(),
	_domain(domain),
	_code(0),
	_redirection(),
	_autoIndexPage(),
	_url(url),
	_queryString(),
	_pathInfo(),
	_inFd(),
	_outFd(),
	_targetType(0),
	_isCgi(false)
{
	extractQueryString(_url, _queryString);
	if (!fixUrl(*this, _url))
		return ;
	addRoot(*this, config);
	this->_code = requestContext._request.setBodyFromHeaders(getMaxClientBodySize());
	if (_code != 0)
		return ;
	if (!checkAllowMeth(*this, method))
		return ;
	if (setRedirection(*this))
		return ;
	if (this->_path[0] != '.')
		this->_path.insert(0, ".");
	_targetType = isDirOrFile(_path);
	if (this->_route != NULL)
	{
		const std::string&	CGIextension = getCgiFileExtension();
		if (CGIextension == "")
			return ;
		if (_targetType != HTTP_NOT_FOUND)
		{
			if (!isExtension(_path, CGIextension))
				return ;
		}
		else if (!setPathInfo(CGIextension, _path))
			return ;
		_code = isCgiExecutable(_path, _targetType);
		if (_code != 0)
			return ;
		_code = setCgiAFdData(requestContext);
	}
}

bool	ARequestType::setPathInfo(const std::string& extension, std::string path)
{
	while (_targetType == HTTP_NOT_FOUND)
	{
		if (path.size() < extension.size() + 1)
			return (false);
		const size_t	pos = path.rfind(extension, path.size() - extension.size() - 1);

		if (pos == std::string::npos)
			return (false);
		const char		afterExtension = path[pos + extension.size()];

		path.erase(pos + extension.size());
		if  (afterExtension == '/')
			_targetType = isDirOrFile(path) ;
	}
	_pathInfo.append(_path, path.size(), std::string::npos);
	_path = path;
	return (true);
}

ARequestType::~ARequestType()
{
}

void	ARequestType::setResponseWithLocation(uint16_t code, const std::string &redirection, bool isReelRedirect)
{
	this->_code = code;
	if (!isReelRedirect)
	{
		const bool	addQueryString = Status::isCodeOfType(code, STATUS_REDIRECTION) && !_queryString.empty();
		size_t		redirectionSize = 7 + _domain.size() + _url.size();

		if (addQueryString)
			redirectionSize += 1 + _queryString.size();
		_redirection.reserve(redirectionSize);
		_redirection.append("http://");
		_redirection.append(_domain);
		_redirection.append(_url);
		if (!addQueryString)
			return ;
		_redirection.append("?");
		_redirection.append(_queryString);
	}
	else
		this->_redirection = redirection;
}

void	ARequestType::setResponse(uint16_t code)
{
	this->_code = code;
}

void	ARequestType::setPath(const std::string &src)
{
	this->_path = src;
}

void	ARequestType::setRoute(const Route *route)
{
	this->_route = route;
}

std::string	&ARequestType::getPath()
{
	return (this->_path);
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

const SharedResource<AFdData*>&	ARequestType::getInFd() const
{
	return (_inFd);
}

const SharedResource<AFdData*>&	ARequestType::getOutFd() const
{
	return (_outFd);
}

bool	ARequestType::getIsCgi(void) const
{
	return (_isCgi);
}

const std::map<uint16_t, std::string>&	ARequestType::getErrorPages(void) const
{
	return (_config.getErrorPages());
}

bool	ARequestType::getAutoIndex(void) const
{
	if (_route == NULL)
		return (false);
	return (_route->getAutoIndex());
}

const std::vector<std::string>&	ARequestType::getIndexs(void) const
{
	if (_route == NULL || _route->getIndex().size() == 0)
		return (_config.getIndex());
	return (_route->getIndex());
}


const std::vector<EMethods>&	ARequestType::getAcceptedMethods(void) const
{
	if (_route == NULL || _route->getAcceptedMethods().size() == 0)
		return (_config.getAcceptedMethods());
	return (_route->getAcceptedMethods());
}

const std::string&	ARequestType::getCgiFileExtension(void) const
{
	if (_route == NULL || _route->getCgiFileExtension() == "")
		return (_config.getCgiFileExtension());
	return (_route->getCgiFileExtension());
}

const std::string&	ARequestType::getCgiInterpreter(void) const
{
	if (_route == NULL || _route->getCgiInterpreter() == "")
		return (_config.getCgiInterpreter());
	return (_route->getCgiInterpreter());
}

const std::string&	ARequestType::getRoot(void) const
{
	if (_route == NULL || _route->getRoot() == "")
		return (_config.getRoot());
	return (_route->getRoot());
}

size_t	ARequestType::getMaxClientBodySize(void) const
{
	if (_route == NULL || _route->getMaxClientBodySize() == (size_t)-1)
		return (_config.getMaxClientBodySize());
	return (_route->getMaxClientBodySize());
}

const std::list<ConfigHeaders>&	ARequestType::getAddHeader(void) const
{
	if (_route == NULL || _route->getAddHeader().size() == 0)
		return (_config.getAddHeader());
	return (_route->getAddHeader());
}

const ServerConfiguration&	ARequestType::getConfig() const
{
	return (_config);
}


const std::string&	ARequestType::getAutoIndexPage(void) const
{
	return (_autoIndexPage);
}

void	ARequestType::setUrl(const std::string &url)
{
	this->_url = url;
}

std::string&	ARequestType::getUrl(void)
{
	return (_url);
}

const std::string&	ARequestType::getUrl(void) const
{
	return (_url);
}

const std::string&	ARequestType::getPath(void) const
{
	return (_path);
}

const std::string&	ARequestType::getQueryString(void) const
{
	return (_queryString);
}

const std::string&	ARequestType::getPathInfo(void) const
{
	return (_pathInfo);
}
