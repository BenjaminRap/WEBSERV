#include <stddef.h>                 // for NULL
#include <stdint.h>                 // for uint16_t
#include <map>                      // for map
#include <string>                   // for string, basic_string
#include <vector>                   // for vector
#include <unistd.h>					// for pipe()

#include "ARequestType.hpp"         // for ARequestType
#include "CgiIn.hpp"				// for CgiIn
#include "CgiOut.hpp"				// for CgiOut
#include "EMethods.hpp"             // for EMethods
#include "Request.hpp"
#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SharedResource.hpp"       // for SharedResource
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_METHOD_NOT...
#include "socketCommunication.hpp"

class AFdData;
class EPollHandler;

bool	checkAllowMeth(const Route &root, EMethods meth);
void	delString(const std::string &toDel, std::string &str);
void	buildNewURl(std::string root, std::string &url);
void	replaceUrl(const std::string &location, const std::string &root, std::string &url);
void	fixPath(std::string &path);
void	fixUrl(ARequestType &req, std::string &url);
void	addRoot(ARequestType &req, const ServerConfiguration &config);
int		execCGI(const char *path, char * const * argv, char * const * env, int& inFd, int& outFd);
void	splitInTwo(const std::string& str, char delimiter, std::string& firstPart, std::string& secondPart);

ARequestType::ARequestType
(
	std::string &url,
	const ServerConfiguration& config,
	EMethods method,
	const std::string& domain,
	RequestContext& requestContext
) :
	_method(method),
	_config(config),
	_route(NULL),
	_path(),
	_domain(domain),
	_code(0),
	_redirection(),
	_autoIndexPage(),
	_url(url),
	_queryString(),
	_inFd(),
	_outFd()
{
	splitInTwo(url, '?', _path, _queryString);
	fixUrl(*this, url);
	if (getCode() == HTTP_BAD_REQUEST)
		return ;
	addRoot(*this, config);
	if (this->_code == HTTP_MOVED_PERMANENTLY || this->_code == HTTP_METHOD_NOT_ALLOWED)
		return;
	if (this->_path[0] != '.')
		this->_path.insert(0, ".");
	if (this->_route != NULL)
	{
		const std::string&	CGIextention = this->_route->getCgiFileExtension();
		if (CGIextention == "" || _path.find(CGIextention) == std::string::npos)
			return ;
		_code = setCgiAFdData(requestContext, CGIextention);
	}
}

bool	setEnv(char *(&env)[20], const Request &request, const std::string& extension, const std::string& path, const std::string& queryString);
bool	setArgv(char* (&argv)[3], const std::string& interpreter, const std::string& cgiFile);
void	deleteArray(const char** array);

uint16_t	ARequestType::setCgiAFdData(RequestContext& requestContext, const std::string& extension)
{
	Request&	request = requestContext.request;
	ABody*		body = request.getBody();
	int			inFd;
	int			outFd;
	char*		env[20];
	char*		argv[3];

	std::memset(env, 0, sizeof(env));
	std::memset(argv, 0, sizeof(argv));
	const bool	error = (!setEnv(env, request, extension, _path, _queryString)
		|| !setArgv(argv, _path, _route->getCgiInterpreter())
		|| execCGI(argv[0], argv, env, inFd, outFd) == -1);

	deleteArray((const char**)env);
	deleteArray((const char**)argv);
	if (error)
		return (HTTP_INTERNAL_SERVER_ERROR);
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
		_config
	), freePointer);
	return (HTTP_OK);
};

ARequestType::~ARequestType()
{
}

void	ARequestType::setResponseWithLocation(uint16_t code, const std::string &redirection, bool isReelRedirect)
{
	this->_code = code;
	this->_redirection = redirection;
	if (!isReelRedirect)
	{
		if ((_route != NULL && redirection.find(_route->getRoot()))
			|| (_config.getRoot() != "" && redirection.find(_config.getRoot())))
		{
			this->_redirection = this->getUrl();
		}
		if (this->_redirection[0] == '.')
			this->_redirection.erase(0, 1);
		this->_redirection = "http://" + this->_domain + this->_redirection;
	}
	if (this->_queryString != "")
		this->_redirection += "?" + this->_queryString;
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

void	ARequestType::setMethod(EMethods method)
{
	this->_method = method;
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

EMethods	ARequestType::getMethod() const
{
	return (this->_method);
}

const SharedResource<AFdData*>&	ARequestType::getInFd() const
{
	return (_inFd);
}

const SharedResource<AFdData*>&	ARequestType::getOutFd() const
{
	return (_outFd);
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
