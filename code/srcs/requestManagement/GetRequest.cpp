#include "GetRequest.hpp"
#include <iostream>
#include <vector>
#include <list>

#define DIRE	1
#define FILE	2
#define NF		3

int							isDirOrFile(const std::string& path);
void						fixPath(std::string &path);
void						directoryCase(GetRequest& get);
std::string					buildPage(std::list<std::string>	&files, const std::string& path);

GetRequest::GetRequest(std::string url, const ServerConfiguration &config) : ARequest(url, config, GET), _autoIndex(false), _index(0)
{
	int			temp;

	if (this->_route != NULL)
		this->setAutoIndex(this->_route->getAutoIndex());
	if (this->_code != 0)
		return ;
	temp = isDirOrFile(this->_url);
	if (temp == DIRE)
		directoryCase(*this);
	else if (temp == FILE)
		setResponse(200, "OK", this->_url);
	else
		setResponse(404, "Not Found", config.getErrorPage(404));
}

GetRequest::GetRequest()
{
	_url = "NULL";
	_autoIndex = false;
	_index = 0;
	_isRoot = false;
	std::cerr << "The default constructor shouldn't be called" << std::endl;
}

GetRequest::~GetRequest()
{
}

GetRequest &GetRequest::operator=(const GetRequest &src)
{
	this->_method = src._method;
	this->_config = src._config;
	this->_route = src._route;
	this->_url = src._url;
	this->_isRoot = src._isRoot;

	this->_code = src._code;
	this->_statusText = src._statusText;
	this->_file = src._file;

	this->_autoIndex = src._autoIndex;
	this->_index = src._index;

	return (*this);
}

GetRequest::GetRequest(const GetRequest &src) : ARequest(src)
{
	if (this == &src)
		return;
	*this = src;
}

bool	GetRequest::getAutoIndex() const
{
	return (this->_autoIndex);
}

std::vector<std::string>	GetRequest::getIndexVec()
{
	return (this->_route->getIndex());
}

void	GetRequest::setAutoIndex(bool src)
{
	this->_autoIndex = src;
}
