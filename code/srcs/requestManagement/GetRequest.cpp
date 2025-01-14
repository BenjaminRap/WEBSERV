#include "GetRequest.hpp"
#include <iostream>
#include <vector>

#define DIRE	1
#define FILE	2
#define NF		3

int							isDirOrFile(std::string path, GetRequest& get);
bool						checkPath(std::string path);
std::string					checkType(const std::string& path, GetRequest& get);
std::vector<std::string>	ls(std::string path);
std::string					fixPath(std::string  path);
std::string					buildPage(std::vector<std::string> files, const std::string& path);
void						directoryCase(GetRequest& get);
void						fixUrl(GetRequest& get, const std::string& url);

/*
 * - Comparaison entre URL et Route
 * 		- Si == alors replace URL par route
 * 			- Set variable en fonction, auto-index, etc ...
 * 		- Sinon continue normal.
*/

GetRequest::GetRequest(const std::string& url)
{
	int			temp;

	this->code = 0;
	this->_index = 0;
	this->_isDirectory = true;
	this->_autoIndex = true;

	fixUrl(*this, url);
	temp = isDirOrFile(this->_url, *this);
	if (temp == DIRE)
		directoryCase(*this);
	else if (temp == FILE)
		setResponse(200, this->_url);
	else
		setResponse(404, "404/page"); //  page a trouver.
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
