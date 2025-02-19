#include "PutRequest.hpp"

#include <iostream>

#define DIRE	1
#define FILE	2
#define NF		3

void						fixPath(std::string &path);
bool						checkAllowMeth(const Route &root, EMethods meth);
void						buildNewURl(std::string root, std::string &url);
void						replaceUrl(const std::string &location, const std::string &root, std::string &url);

void	fixUrl(PutRequest &put, std::string &url)
{
	if (*url.begin() != '/')
		put.setResponse(400, "Bad Request", "Bad Request");
	else
	{
		fixPath(url);
		put.setUrl(url);
	}
}

void	addRoot(PutRequest &put, const ServerConfiguration& config)
{
	const Route	*temp = config.getOneRoutes(put.getUrl());

	if (temp == NULL)
	{
		buildNewURl(config.getRoot(), put.getUrl());
		return ;
	}
	put.setRoot(temp);
	put.setIsRoot(true);
	if (!checkAllowMeth(*temp, POST))
	{
		put.setResponse(405, "Not Allowed", config.getErrorPage(405));
		return ;
	}
	const std::string &redir = temp->getRedirection().url;
	if (!redir.empty())
		put.setResponse(301, "Moved Permanently", redir);
	else
		replaceUrl(config.getLocation(put.getUrl()), temp->getRoot(), put.getUrl());
}

bool	checkFileName(const std::string &fileName)
{
	if (fileName.empty())
		return (false);
	if (fileName.find('/') != std::string::npos)
		return (false);
	return (true);
}


void	PutRequest::parsing(std::string &url, const ServerConfiguration &config)
{
	this->_config = &config;
	fixUrl(*this, url);
	if (this->code == 400)
		return ;
	addRoot(*this, config);
	if (this->code == 301 || this->code == 405)
		return ;
	if (this->_url[0] != '.')
		this->_url.insert(0, ".");
}

PutRequest::PutRequest(std::string url, std::string fileName, const ServerConfiguration &config) : _isRoot(false), code(0), fd(0)
{
	std::string path;

	parsing(url, config);
	if (this->code != 0)
		return ;
	if (!checkFileName(fileName))
	{
		this->setResponse(400, "Bad Request", "Bad Request");
		return ;
	}
	path = this->_url + fileName;
	this->fd = open(path.c_str(), O_CREAT | O_EXCL , 0666);
	if (errno == EEXIST)
		this->setResponse(409, "Conflict", "Conflict");
	else if (fd == -1)
	{
		if (errno == EACCES)
			this->setResponse(403, "Forbidden", this->getError(403));
		else
			this->setResponse(500, "Internal Server Error", this->getError(500));
	}
	else
		this->setResponse(201, "Created", path);
}

PutRequest::PutRequest() : _config(), _root()
{
	_url = "NULL";
	code = 0;
	_isRoot = false;
	std::cerr << "The default constructor shouldn't be called" << std::endl;
}

PutRequest::~PutRequest()
{
}

PutRequest	&PutRequest::operator=(const PutRequest &src)
{
	this->_url = src._url;
	this->code = src.code;
	this->_config = src._config;
	this->_root = src._root;
	this->file = src.file;

	return (*this);
}

PutRequest::PutRequest(const PutRequest &src) : _config(), _root(), code()
{
	if (this == &src)
		return;
	*this = src;
}

void	PutRequest::setResponse(int newcode, const std::string &status, const std::string& newfile)
{
	this->code = newcode;
	this->statusText = status;
	this->file = newfile;
}

void	PutRequest::setUrl(const std::string &src)
{
	this->_url = src;
}

std::string	&PutRequest::getUrl()
{
	return (this->_url);
}

void	PutRequest::setRoot(const Route *root)
{
	this->_root = root;
}

void	PutRequest::setIsRoot(bool src)
{
	this->_isRoot = src;
}

bool	PutRequest::getIsRoot() const
{
	return (this->_isRoot);
}

std::string	PutRequest::getError(unsigned short error)
{
	return (this->_config->getErrorPage(error));
}