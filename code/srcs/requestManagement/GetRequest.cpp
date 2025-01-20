#include "GetRequest.hpp"
#include <iostream>
#include <vector>

#define DIRE	1
#define FILE	2
#define NF		3

int							isDirOrFile(const std::string& path, GetRequest& get);
bool						checkPath(std::string path);
std::string					checkType(const std::string& path, GetRequest& get);
std::string					fixPath(std::string  path);
std::string					buildPage(std::vector<std::string> files, const std::string& path);
void						directoryCase(GetRequest& get);
void						fixUrl(GetRequest& get, const std::string& url);


bool	checkAllowMeth(Route root)
{
	std::vector<EMethods> meths;
	size_t			len;

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

std::string replaceUrl(std::string location, std::string root, std::string url)
{
	std::string temp;
	size_t		found;

	found = location.find(root);
	while (found != std::string::npos)
	{
		temp = location.substr(0, found);
		temp += url;
		temp += location.substr(found + root.length());
		location = temp;
		found = location.find(root);
	}
	return (location);
}


void	addRoot(GetRequest get, ServerConfiguration config)
{
	std::map<std::string, Route> 			roots;
	std::map<std::string, Route>::iterator	it;
	SRedirection							redirectSt;

	roots = config.getRoutes();
	it = roots.find(get.getUrl());

	if (it == roots.end())
		get.setUrl(config.getRoot() + get.getUrl());
	else
	{
		if (!checkAllowMeth(it->second))
		{
			get.setResponse(405, config.getErrorPage(405));
			return ;
		}
		std::cout << "Root find" << std::endl;
		redirectSt = it->second.getRedirection();
		if (!redirectSt.url.empty())
			get.setResponse(301, redirectSt.url);
		else
		{
			std::cout << "Root Set" << std::endl;
			get.setRoot(it->second);
			get.setAutoIndex(it->second.getAutoIndex());
			get.setUrl(replaceUrl(it->first, it->second.getRoot(), get.getUrl()));
		}
	}
}


GetRequest::GetRequest(const std::string& url, ServerConfiguration config)
{
	int			temp;

	this->code = 0;
	this->_index = 0;
	this->_isDirectory = true;
	this->_config = &config;

	fixUrl(*this, url);
	addRoot(*this, config);
	std::cout << this->_root << std::endl;
	std::cout << "Zebi" << std::endl;
	temp = isDirOrFile(this->_url, *this);
	if (temp == DIRE)
		directoryCase(*this);
	else if (temp == FILE)
		setResponse(200, this->_url);
	else
		setResponse(404, config.getErrorPage(404));
}

GetRequest::GetRequest()
{
	_url = "NULL";
	_autoIndex = false;
	_isDirectory = false;
	_index = 0;
	code = 0;
	std::cerr << "The default constructor shouldn't be called" << std::endl;
}

GetRequest::~GetRequest()
{
}

GetRequest &GetRequest::operator=(const GetRequest &src)
{
	this->_index = src._index;
	this->_isDirectory = src._isDirectory;
	this->_autoIndex = src._autoIndex;
	this->_url = src._url;
	this->code = src.code;
	this->file = src.file;

	return (*this);
}

GetRequest::GetRequest(const GetRequest &src) : _autoIndex(), _isDirectory(), _index(), code()
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

void GetRequest::setIsDirectory(bool src)
{
	this->_isDirectory = src;
}

bool GetRequest::getIsDirectory() const
{
	return (this->_isDirectory);
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

void	GetRequest::setRoot(Route root)
{
	this->_root = &root;
}