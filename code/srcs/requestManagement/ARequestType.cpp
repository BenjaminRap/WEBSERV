#include <stddef.h>                 // for NULL, size_t
#include <stdint.h>                 // for uint16_t
#include <map>                      // for map
#include <string>                   // for string, basic_string
#include <vector>                   // for vector
#include <unistd.h>					// for pipe()

#include "ARequestType.hpp"         // for ARequestType
#include "EMethods.hpp"             // for EMethods
#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "SharedResource.hpp"       // for SharedResource
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_METHOD_NOT...

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
	_redirection(),
	_autoIndexPage(),
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
	if (this->_route != NULL)
	{
		const std::string	CGIextention = this->_route->getCgiFileExtension();
		if (!CGIextention.empty())
		{
			char		**env = getEnv();
			int			fd[2];
			char		*argv[] = {const_cast<char *>(this->_url.c_str()), NULL};
			const char *path = this->_url.c_str();

			if (pipe(fd))
			{
				this->_code = HTTP_INTERNAL_SERVER_ERROR;
			}
			this->_inFd = SharedResource(fd[1], close);
			this->_outFd = SharedResource(fd[0], close);
			pid_t	pid = execCGI(path, argv, env, fd);
			if (pid == -1)
			{
				this->_code = HTTP_INTERNAL_SERVER_ERROR;
			}
		}
	}
}

ARequestType::~ARequestType()
{
}

void	ARequestType::setRedirectionResponse(uint16_t code, const std::string &redirection)
{
	this->_code = code;
	this->_redirection = redirection;
}


void	ARequestType::setResponse(uint16_t code)
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

SharedResource<int>	ARequestType::getInFd() const
{
	return (_inFd);
}

SharedResource<int>	ARequestType::getOutFd() const
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


const std::string&	ARequestType::getAutoIndexPage(void) const
{
	return (_autoIndexPage);
}
