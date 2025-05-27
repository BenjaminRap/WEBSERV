#include <stdint.h>                 // for uint16_t
#include <sys/types.h>              // for pid_t
#include <cstring>                  // for NULL, memset
#include <exception>                // for exception
#include <map>                      // for map
#include <string>                   // for allocator, string, basic_string
#include <vector>                   // for vector

#include "ARequestType.hpp"         // for ARequestType
#include "CgiIn.hpp"                // for CgiIn
#include "CgiOut.hpp"               // for CgiOut
#include "EMethods.hpp"             // for EMethods
#include "Request.hpp"              // for Request
#include "RequestContext.hpp"       // for RequestContext
#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SharedResource.hpp"       // for freePointer, SharedResource
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_INTERNAL_S...
#include "socketCommunication.hpp"  // for closeFdAndPrintError

class ABody;
class AFdData;  // lines 20-20

bool		checkAllowMeth(ARequestType& req, EMethods meth);
void		delString(const std::string &toDel, std::string &str);
void		buildNewURl(std::string root, std::string &url);
void		replaceUrl(const std::string &location, const std::string &root, std::string &url);
void		fixPath(std::string &path);
bool		fixUrl(ARequestType &req, std::string &url);
void		addRoot(ARequestType &req, const ServerConfiguration &config);
int			execCGI(const char *path, char * const * argv, char * const * env, int& inFd, int& outFd);
void		extractQueryString(std::string& url, std::string& queryString);
uint16_t	isCgiExecutable(const std::string& path, uint16_t targetType);
bool		setEnv(char *(&env)[20], const Request &request, const std::string& extension, const std::string& path, const std::string& queryString, RequestContext& requestContext, const std::string& pathInfo);
bool		setArgv(char* (&argv)[3], const std::string& interpreter, const std::string& cgiFile);
void		deleteArray(const char** array);
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
	_outFd()
{
	extractQueryString(url, _queryString);
	if (!fixUrl(*this, url))
		return ;
	addRoot(*this, config);
	this->_code = requestContext.request.setBodyFromHeaders(getMaxClientBodySize());
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
		_code = setCgiAFdData(requestContext, CGIextension);
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
	std::cout << "path :" << _path << std::endl;
	std::cout << "pathInfo :" << _pathInfo << std::endl;
	return (true);
}

uint16_t	ARequestType::setCgiAFdData(RequestContext& requestContext, const std::string& extension)
{
	Request&	request = requestContext.request;
	ABody*		body = request.getBody();
	int			inFd;
	int			outFd;
	char*		env[20];
	char*		argv[3];
	pid_t		pid;

	std::memset(env, 0, sizeof(env));
	std::memset(argv, 0, sizeof(argv));
	try
	{
		const bool	error = (!setEnv(env, request, extension, _path, _queryString, requestContext, _pathInfo)
			|| !setArgv(argv, getCgiInterpreter(), _path)
			|| (pid = execCGI(argv[0], argv, env, inFd, outFd)) == -1);
		deleteArray((const char**)env);
		deleteArray((const char**)argv);
		if (error)
			return (HTTP_INTERNAL_SERVER_ERROR);
	}
	catch (const std::exception& e)
	{
		deleteArray((const char**)env);
		deleteArray((const char**)argv);
		throw;
	}
	if (body != NULL)
	{
		this->_inFd.setManagedResource(new CgiIn(
			inFd,
			requestContext.ePollHandler,
			requestContext.requestBuff,
			*body,
			requestContext.connectedSocketData,
			requestContext.response
		), freePointer);
	}
	else
		closeFdAndPrintError(inFd);
	this->_outFd.setManagedResource(new CgiOut(
		outFd,
		requestContext.ePollHandler,
		requestContext.responseBuff,
		_config,
		pid
	), freePointer);
	return (HTTP_OK);
};

ARequestType::~ARequestType()
{
}

void	ARequestType::setResponseWithLocation(uint16_t code, const std::string &redirection, bool isReelRedirect)
{
	this->_code = code;
	if (!isReelRedirect)
	{
		_redirection.reserve(7 + _domain.size() + _url.size() + _queryString.size());
		_redirection.append("http://");
		_redirection.append(_domain);
		_redirection.append(_url);
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
