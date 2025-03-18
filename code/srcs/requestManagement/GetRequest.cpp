#include <fcntl.h>                  // for open, O_RDONLY
#include <stddef.h>                 // for NULL
#include <list>                     // for list
#include <string>                   // for string, basic_string
#include <vector>                   // for vector

#include "ARequestType.hpp"         // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"             // for EMethods
#include "GetRequest.hpp"           // for GetRequest
#include "Route.hpp"                // for Route
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "requestStatusCode.hpp"    // for HTTP_OK, HTTP_INTERNAL_SERVER_ERROR

int							isDirOrFile(const std::string& path);
void						fixPath(std::string &path);
void						directoryCase(GetRequest& get);
std::string					buildPage(std::list<std::string>	&files, const std::string& path);

GetRequest::GetRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, GET), _autoIndex(false), _index(0)
{
	int			targetType;

	if (this->_route != NULL)
		this->setAutoIndex(this->_route->getAutoIndex());
	if (this->_code != 0)
		return ;
	targetType = isDirOrFile(this->_url);
	if (targetType == DIRE)
		directoryCase(*this);
	else if (targetType == LS_FILE)
		setResponse(HTTP_OK);
	else
		setResponse(HTTP_NOT_FOUND);
	if (this->_code == HTTP_OK)
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
