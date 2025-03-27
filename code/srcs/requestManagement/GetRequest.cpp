#include <fcntl.h>                  // for open, O_RDONLY
#include <stddef.h>                 // for NULL
#include <list>                     // for list
#include <string>                   // for string, basic_string

#include "ARequestType.hpp"         // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"             // for EMethods
#include "GetRequest.hpp"           // for GetRequest
#include "ServerConfiguration.hpp"  // for ServerConfiguration
#include "requestStatusCode.hpp"    // for HTTP_OK, HTTP_INTERNAL_SERVER_ERROR
#include "socketCommunication.hpp"	// for checkError

int							isDirOrFile(const std::string& path);
void						fixPath(std::string &path);
void						directoryCase(GetRequest& get);
std::string					buildPage(std::list<std::string>	&files, const std::string& path);
ssize_t						getFileSize(const std::string &filePath);

GetRequest::GetRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, GET)
{
	int			targetType;

	if (this->_code != 0)
		return ;
	targetType = isDirOrFile(this->_url);
	if (targetType == DIRE)
		directoryCase(*this);
	else if (targetType == LS_FILE)
		setResponse(HTTP_OK);
	else
		setResponse(HTTP_NOT_FOUND);
	if (this->_code == HTTP_OK && _autoIndexPage.size() == 0)
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


void	GetRequest::setResponseWithAutoIndex(uint16_t code, const std::string &autoIndexPage)
{
	this->_code = code;
	this->_autoIndexPage = autoIndexPage;
}
