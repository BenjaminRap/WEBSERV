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

GetRequest::~GetRequest()
{
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
