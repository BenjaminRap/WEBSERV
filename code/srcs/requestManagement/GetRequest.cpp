#include "GetRequest.hpp"
#include <iostream>
#include <vector>
#include <list>

#define DIRE	1
#define FILE	2
#define NF		3

int							isDirOrFile(const std::string& path);
void						checkType(std::string& path, GetRequest& get);
void						fixPath(std::string &path);
std::string					buildPage(std::list<std::string>	&files, const std::string& path);
void						directoryCase(GetRequest& get);
void						fixUrl(GetRequest& get, std::string& url);



bool	checkAllowMeth(const Route &root)
{
	const std::vector<EMethods>	&meths = root.getAcceptedMethods();
	size_t						len;

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

void replaceUrl(const std::string& location, const std::string& root, std::string &url)
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

void	buildNewURl(std::string root, std::string &url)
{
	if (!root.empty() && root[root.size() - 1] == '/')
		root.erase(root.size() - 1);
	url.insert(0, root);
}

void	addRoot(GetRequest &get, const ServerConfiguration& config)
{
	const Route	*temp = config.getOneRoutes(get.getUrl());

	if (temp == NULL)
	{
		buildNewURl(config.getRoot(), get.getUrl());
		return ;
	}
	get.setRoot(temp);
	get.setIsRoot(true);
	if (!checkAllowMeth(*temp))
	{
		get.setResponse(405, config.getErrorPage(405));
		return ;
	}
	const std::string &redir = temp->getRedirection().url;
	if (!redir.empty())
		get.setResponse(301, redir);
	else
	{
		get.setAutoIndex(temp->getAutoIndex());
		replaceUrl(get.getUrl(), temp->getRoot(), get.getUrl()); // ici
	}
}

GetRequest::GetRequest(std::string url, const ServerConfiguration &config) : _autoIndex(false), _index(0), _isRoot(false), code(0)
{
	int			temp;

	this->_config = &config;
	fixUrl(*this, url);
	if (this->code == 400)
		return ;
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

std::string &GetRequest::getUrl()
{
	return (this->_url);
}

std::vector<std::string> GetRequest::getIndexVec()
{
	return (this->_root->getIndex());
}

void	GetRequest::setRoot(const Route *root)
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

std::string	GetRequest::getError(unsigned short error)
{
	return (this->_config->getErrorPage(error));
}