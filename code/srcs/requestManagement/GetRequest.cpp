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
#include "socketCommunication.hpp"	// for checkError

int							isDirOrFile(const std::string& path);
void						fixPath(std::string &path);
void						directoryCase(GetRequest& get);
std::string					buildPage(std::list<std::string>	&files, const std::string& path);
ssize_t						getFileSize(const std::string &filePath);

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
		const int fd = open(this->_url.c_str(), O_RDONLY);
		if (checkError(fd, -1, "open() : "))
		{
			this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
			return ;
		}
		this->_outFd.setManagedResource(fd, closeFdAndPrintError);
		const ssize_t fileSize = getFileSize(this->_url.c_str());
		if (fileSize == -1)
		{
			this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
			return ;
		}
		this->_outSize = fileSize;
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
