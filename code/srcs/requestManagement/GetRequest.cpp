#include "GetRequest.hpp"
#include <iostream>
#include <vector>

#define DIRE	1
#define FILE	2
#define NF		3

int							isDirOrFile(const std::string& path);
bool						checkPath(std::string path);
std::string					checkType(const std::string& path, GetRequest& get);
std::string					fixPath(std::string  path);
std::string					buildPage(std::vector<std::string> files, const std::string& path);
void						directoryCase(GetRequest& get);
void						fixUrl(GetRequest& get, const std::string& url);


bool	checkAllowMeth(const Route& root)
{
	std::vector<EMethods>	meths;
	size_t					len;

	meths = root.getAcceptedMethods();
	len = meths.size();
	if (len == 0)
		return (true);
	for (size_t i = 0; i < len ; i++)
	{
		if (meths[i] == GET)
			return (true);
	}
	return (false);
}

std::string	replaceUrl(const std::string& location, const std::string& root, std::string url)
{
	std::string	temp;
	size_t		found;

	if (root.empty())
		return (url);
	found = url.find(location);
	while (found == 0)
	{
		temp = url.substr(0, found);
		temp += root;
		temp += url.substr(found + location.length());
		url = temp;
		found = url.find(location, found + 1);
		if (found == url.size() - 1)
			break;
	}
	return (url);
}

std::string	buildNewURl(std::string root, std::string url)
{
	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	url = root + url;
	return (url);
}

void	addRoot(GetRequest &get, const ServerConfiguration& config)
{
	std::map<std::string, Route> 			roots;
	std::map<std::string, Route>::iterator	it;

	roots = config.getRoutes();
	it = roots.find(get.getUrl());
	if (it == roots.end())
	{
		get.setUrl(buildNewURl(config.getRoot(), get.getUrl()));
		return ;
	}
	get.setRoot(config.getOneRoutes(it->first));
	get.setIsRoot(true);
	if (!checkAllowMeth(it->second))
	{
		get.setResponse(405, "Method Not Allowed");
		return ;
	}
	if (!it->second.getRedirection().url.empty())
		get.setResponse(301, it->second.getRedirection().url);
	else
	{
		get.setAutoIndex(it->second.getAutoIndex());
		get.setUrl(replaceUrl(it->first, it->second.getRoot(), get.getUrl()));
	}
}

GetRequest::GetRequest(const std::string& url, ServerConfiguration config) : _autoIndex(false), _index(0), _isRoot(false), code(0)
{
	int			temp;

	this->_config = &config;
	fixUrl(*this, url);
	addRoot(*this, config);
	if (this->code == 301 || this->code == 405)
		return ;
	if (this->_url[0] != '.')
		this->_url = "." + this->_url;
	temp = isDirOrFile(this->_url);
	if (temp == DIRE)
		directoryCase(*this);
	else if (temp == FILE)
		setResponse(200, this->_url);
	else
		setResponse(404, config.getErrorPage(404));
}

GetRequest::GetRequest() : _config(), _root()
{
	_url = "NULL";
	_autoIndex = false;
	_index = 0;
	code = 0;
	_isRoot = false;
	std::cerr << "The default constructor shouldn't be called" << std::endl;
}

GetRequest::~GetRequest()
{
	if (this->_isRoot)
		delete this->_root;
}

GetRequest &GetRequest::operator=(const GetRequest &src)
{
	this->_url = src._url;
	this->_autoIndex = src._autoIndex;
	this->_index = src._index;
	this->code = src.code;
	this->_config = src._config;
	this->_root = src._root;
	this->file = src.file;

	return (*this);
}

GetRequest::GetRequest(const GetRequest &src) : _config(), _root(), _autoIndex(), _index(), code()
{
	if (this == &src)
		return;
	*this = src;
}

void GetRequest::setResponse(int newcode, const std::string& newfile)
{
	this->code = newcode;
	this->file = newfile;
}

bool GetRequest::getAutoIndex() const
{
	return (this->_autoIndex);
}

void GetRequest::setAutoIndex(bool src)
{
	this->_autoIndex = src;
}

void GetRequest::setUrl(const std::string& src)
{
	this->_url = src;
}

std::string GetRequest::getUrl()
{
	return (this->_url);
}

std::vector<std::string> GetRequest::getIndexVec()
{
	return (this->_root->getIndex());
}

void	GetRequest::setRoot(Route *root)
{
	this->_root = root;
}

void GetRequest::setIsRoot(bool src)
{
	this->_isRoot = src;
}

bool GetRequest::getIsRoot() const
{
	return (this->_isRoot);
}