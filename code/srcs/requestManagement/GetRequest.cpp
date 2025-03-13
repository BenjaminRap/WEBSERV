#include "GetRequest.hpp"
#include <vector>
#include <list>

int							isDirOrFile(const std::string& path);
void						fixPath(std::string &path);
void						directoryCase(GetRequest& get);
std::string					buildPage(std::list<std::string>	&files, const std::string& path);

GetRequest::GetRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, GET), _autoIndex(false), _index(0)
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
		setResponse(200, this->_url);
	else
		setResponse(404, config.getErrorPage(404));
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
