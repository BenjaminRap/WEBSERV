#include "DeleteRequest.hpp"

#include <iostream>

#define DIRE	1
#define FILE	2
#define NF		3

int							isDirOrFile(const std::string& path);
void						fixPath(std::string &path);
int							directoryCase(const std::string &path, DeleteRequest &del);
bool						checkAllowMeth(const Route &root, EMethods meth);
void						buildNewURl(std::string root, std::string &url);
void						replaceUrl(const std::string &location, const std::string &root, std::string &url);
int							fileCase(const std::string &path, DeleteRequest &del);

void	fixUrl(DeleteRequest &del, std::string &url)
{
	if (*url.begin() != '/')
		del.setResponse(400, "Bad Request", "Bad Request");
	else
	{
		fixPath(url);
		del.setUrl(url);
	}
}

void	addRoot(DeleteRequest &del, const ServerConfiguration& config)
{
	const Route	*temp = config.getOneRoutes(del.getUrl());

	if (temp == NULL)
	{
		buildNewURl(config.getRoot(), del.getUrl());
		return ;
	}
	del.setRoot(temp);
	del.setIsRoot(true);
	if (!checkAllowMeth(*temp, DELETE))
	{
		del.setResponse(405, "Method Not Allowed", config.getErrorPage(405));
		return ;
	}
	const std::string &redir = temp->getRedirection().url;
	if (!redir.empty())
		del.setResponse(301, "Moved Permanently", redir);
	else
		replaceUrl(config.getLocation(del.getUrl()), temp->getRoot(), del.getUrl());
}

void	DeleteRequest::parsing(std::string &url, const ServerConfiguration &config)
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

DeleteRequest::DeleteRequest(std::string url, const ServerConfiguration &config) : _isRoot(false), code(0)
{
	int			temp;

	parsing(url, config);
	if (this->code != 0)
		return ;
	temp = isDirOrFile(this->_url);
	if (temp == DIRE)
		directoryCase(this->_url, *this);
	else if (temp == FILE)
		fileCase(this->_url, *this);
	else if (temp == -1)
		this->setResponse(403, "Forbidden", "Forbidden");
	else
		setResponse(404, "Not Found", config.getErrorPage(404));
}

DeleteRequest::DeleteRequest() : _config(), _root()
{
	_url = "NULL";
	code = 0;
	_isRoot = false;
	std::cerr << "The default constructor shouldn't be called" << std::endl;
}

DeleteRequest::~DeleteRequest()
{
}

DeleteRequest	&DeleteRequest::operator=(const DeleteRequest &src)
{
	this->_url = src._url;
	this->code = src.code;
	this->_config = src._config;
	this->_root = src._root;
	this->file = src.file;

	return (*this);
}

DeleteRequest::DeleteRequest(const DeleteRequest &src) : _config(), _root(), code()
{
	if (this == &src)
		return;
	*this = src;
}

void	DeleteRequest::setResponse(int newcode, const std::string &status, const std::string& newfile)
{
	this->code = newcode;
	this->statusText = status;
	this->file = newfile;
}

void	DeleteRequest::setUrl(const std::string &src)
{
	this->_url = src;
}

std::string	&DeleteRequest::getUrl()
{
	return (this->_url);
}

void	DeleteRequest::setRoot(const Route *root)
{
	this->_root = root;
}

void	DeleteRequest::setIsRoot(bool src)
{
	this->_isRoot = src;
}

bool	DeleteRequest::getIsRoot() const
{
	return (this->_isRoot);
}

std::string	DeleteRequest::getError(unsigned short error)
{
	return (this->_config->getErrorPage(error));
}