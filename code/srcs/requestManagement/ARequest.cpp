#include "ARequest.hpp"

ARequest::ARequest() : _method(GET), _config(NULL), _root(NULL), _url(""), _code(0), _statusText(""), _file("")
{
	return ;
}

bool	checkAllowMeth(const Route &root, EMethods meth)
{
	const std::vector<EMethods>	&meths = root.getAcceptedMethods();
	size_t						len;

	len = meths.size();
	if (len == 0)
		return (true);
	for (size_t i = 0; i < len ; i++)
	{
		if (meths[i] == meth)
			return (true);
	}
	return (false);
}

void	delString(const std::string& toDel, std::string &str)
{
	size_t	found;
	size_t	len;

	len = toDel.length();
	found = str.find(toDel, 0);
	while (found != std::string::npos)
	{
		str.erase(found, len);
		found = str.find(toDel, 0);
	}
}

void	buildNewURl(std::string root, std::string &url)
{
	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	url.insert(0, root);
}

void	replaceUrl(const std::string& location, const std::string& root, std::string &url)
{
	size_t found;

	if (root.empty())
		return ;
	found = url.find(location);
	while (found != std::string::npos)
	{
		url.replace(found, location.length(), root);
		found = url.find(location, found + root.length());
	}
}

void	fixPath(std::string &path)
{
	size_t	found;
	size_t	foundBack;

	found = path.find("/../", 0);
	while (found != std::string::npos)
	{
		if (found == 0)
			path.erase(0, 3);
		else
		{
			foundBack = path.find_last_of('/', found - 1);
			path.erase(foundBack, found - foundBack + 3);
		}
		found = path.find("/../", 0);
	}
	delString("./", path);
	delString("//", path);
	if (path.empty())
		path = "/";
}

void	fixUrl(ARequest &req, std::string &url)
{
	if (*url.begin() != '/')
		req.setResponse(400, "Bad Request", "Bad Request");
	else
	{
		fixPath(url);
		req.setUrl(url);
	}
}

void	addRoot(ARequest &get, const ServerConfiguration& config)
{
	const Route	*temp = config.getOneRoutes(get.getUrl());

	if (temp == NULL)
	{
		buildNewURl(config.getRoot(), get.getUrl());
		return ;
	}
	get.setRoot(temp);
	get.setIsRoot(true);
	if (!checkAllowMeth(*temp, GET))
	{
		get.setResponse(405, "Not Allowed", config.getErrorPage(405));
		return ;
	}
	const std::string &redir = temp->getRedirection().url;
	if (!redir.empty())
		get.setResponse(301, "Moved Permanently", redir);
	else
	{
//		if (get.getMethod() == GET) a mettre dans GET directement
//			get.setAutoIndex(temp->getAutoIndex());
		replaceUrl(config.getLocation(get.getUrl()), temp->getRoot(), get.getUrl());
	}
}


ARequest::ARequest(std::string url, const ServerConfiguration&config, EMethods method) : _config(&config), _url(url), _method(method)
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
	*this = src;
}

ARequest& ARequest::operator=(const ARequest& src)
{
	if (this != &src)
	{
		this->_config = src._config;
		this->_root = src._root;
		this->_url = src._url;
		this->_method = src._method;
		this->_code = src._code;
		this->_statusText = src._statusText;
		this->_file = src._file;
	}
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
