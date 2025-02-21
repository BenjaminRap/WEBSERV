#include "ARequest.hpp"


bool	checkAllowMeth(const Route &root, EMethods meth);
void	delString(const std::string& toDel, std::string &str);
void	buildNewURl(std::string root, std::string &url);
void	replaceUrl(const std::string& location, const std::string& root, std::string &url);
void	fixPath(std::string &path);
void	fixUrl(ARequest &req, std::string &url);
void	addRoot(ARequest &get, const ServerConfiguration& config);

ARequest::ARequest() : _method(GET), _config(NULL), _root(NULL), _url(""), _code(0), _statusText(""), _file("")
{
	return ;
}

ARequest::ARequest(std::string url, const ServerConfiguration&config, EMethods method) : _method(method), _config(&config), _root(NULL), _url(url), _isRoot(false), _code(0)
{
	fixUrl(*this, url);
	if (getCode() == 400)
		return ;
	addRoot(*this, config);
	if (this->_code == 301 || this->_code == 405)
		return ;
	if (this->_url[0] != '.')
		this->_url.insert(0, ".");
}

ARequest::~ARequest()
{
	return ;
}

ARequest::ARequest(const ARequest& src)
{
	if (this != &src)
		return ;
	*this = src;
}

ARequest& ARequest::operator=(const ARequest& src)
{
	this->_config = src._config;
	this->_root = src._root;
	this->_url = src._url;
	this->_method = src._method;
	this->_isRoot = src._isRoot;

	this->_code = src._code;
	this->_statusText = src._statusText;
	this->_file = src._file;

	return (*this);
}

void	ARequest::setResponse(int code, const std::string &status, const std::string& file)
{
	this->_code = code;
	this->_statusText = status;
	this->_file = file;
}

void	ARequest::setUrl(const std::string& src)
{
	this->_url = src;
}

void	ARequest::setRoot(const Route *root)
{
	this->_root = root;
}

void	ARequest::setMethod(EMethods method)
{
	this->_method = method;
}

void ARequest::setIsRoot(bool src)
{
	this->_isRoot = src;
}

bool ARequest::getIsRoot() const
{
	return (this->_isRoot);
}


std::string	&ARequest::getUrl()
{
	return (this->_url);
}

const std::string	&ARequest::getStatusText() const
{
	return (this->_statusText);
}

const std::string	&ARequest::getFile() const
{
	return (this->_file);
}

const Route	*ARequest::getRoot() const
{
	return (this->_root);
}

int	ARequest::getCode() const
{
	return (this->_code);
}

EMethods	ARequest::getMethod() const
{
	return (this->_method);
}

const std::string	&ARequest::getError(unsigned short error)
{
	return (this->_config->getErrorPage(error));
}