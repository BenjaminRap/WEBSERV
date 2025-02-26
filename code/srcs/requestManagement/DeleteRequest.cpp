#include "DeleteRequest.hpp"
#include <iostream>

#define DIRE	1
#define FILE	2
#define NF		3

int							isDirOrFile(const std::string& path);
int							directoryCase(const std::string &path, DeleteRequest &del);
int							fileCase(const std::string &path, DeleteRequest &del);

DeleteRequest::DeleteRequest(std::string url, const ServerConfiguration &config) : ARequest(url, config, DELETE)
{
	int			temp;

	if (this->_code != 0)
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

DeleteRequest::~DeleteRequest()
{
}

DeleteRequest	&DeleteRequest::operator=(const DeleteRequest &src)
{
	this->_config = src._config;
	this->_route = src._route;
	this->_url = src._url;
	this->_method = src._method;
	this->_isRoot = src._isRoot;

	this->_code = src._code;
	this->_statusText = src._statusText;
	this->_file = src._file;

	return (*this);
}
