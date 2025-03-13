#include "ARequestType.hpp"

bool	checkAllowMeth(const Route &root, EMethods meth);
void	delString(const std::string &toDel, std::string &str);
void	buildNewURl(std::string root, std::string &url);
void	replaceUrl(const std::string &location, const std::string &root, std::string &url);
void	fixPath(std::string &path);
void	fixUrl(ARequestType &req, std::string &url);
void	addRoot(ARequestType &get, const ServerConfiguration &config);

ARequestType::ARequestType(std::string &url, const ServerConfiguration& config, EMethods method) : _method(method), _config(&config), _route(NULL), _url(url), _isRoute(false), _code(0), _redirection("")
{
	fixUrl(*this, url);
	if (getCode() == 400)
		return ;
	addRoot(*this, config);
	if (this->_code == 301 || this->_code == 405)
		return;
	if (this->_url[0] != '.')
		this->_url.insert(0, ".");
}

ARequestType::~ARequestType()
{
	return ;
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

void ARequestType::setIsRoute(bool src)
{
	this->_isRoute = src;
}

bool ARequestType::getIsRoute() const
{
	return (this->_isRoute);
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

const std::string	&ARequestType::getError(unsigned short error)
{
	return (this->_config->getErrorPage(error));
}
