#include <vector>
#include <list>

#include "GetRequest.hpp"
#include "requestStatusCode.hpp"

int							isDirOrFile(const std::string& path);
void						fixPath(std::string &path);
void						directoryCase(GetRequest& get);
std::string					buildPage(std::list<std::string>	&files, const std::string& path);

GetRequest::GetRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, GET), _autoIndex(false), _index(0), _fd(-1)
{
	int			temp;

	if (this->_route != NULL)
		this->setAutoIndex(this->_route->getAutoIndex());
	if (this->_code != 0)
		return ;
	temp = isDirOrFile(this->_url);
	if (temp == DIRE)
		directoryCase(*this);
	else if (temp == LS_FILE)
		setResponse(HTTP_OK);
	else
		setResponse(HTTP_NOT_FOUND);
	if (this->_code == 200)
	{
		this->_fd = open(this->_url.c_str(), O_RDONLY);
		if (this->_fd == -1)
			this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
	}
}

GetRequest::~GetRequest()
{
}

bool	GetRequest::getAutoIndex() const
{
	return (this->_autoIndex);
}

const std::vector<std::string>	&GetRequest::getIndexVec()
{
	return (this->_route->getIndex());
}

const std::vector<std::string>	&GetRequest::getDefaultIndexVec()
{
	return (this->_config->getIndex());
}

void	GetRequest::setAutoIndex(bool src)
{
	this->_autoIndex = src;
}

int		GetRequest::getFd() const
{
	return (this->_fd);
}

void	GetRequest::setFd(int fd)
{
	this->_fd = fd;
}