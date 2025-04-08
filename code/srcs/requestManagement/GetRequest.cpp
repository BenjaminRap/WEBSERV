#include <fcntl.h>                  // for open, O_RDONLY
#include <stdint.h>                 // for uint16_t
#include <sys/types.h>              // for ssize_t
#include <string>                   // for basic_string, string

#include "ARequestType.hpp"         // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"             // for EMethods
#include "GetRequest.hpp"           // for GetRequest
#include "SharedResource.hpp"       // for SharedResource
#include "requestStatusCode.hpp"    // for HTTP_INTERNAL_SERVER_ERROR, HTTP_OK
#include "socketCommunication.hpp"  // for checkError, closeFdAndPrintError

class ServerConfiguration;

uint16_t	isDirOrFile(const std::string& path);
void		directoryCase(GetRequest& get);
ssize_t		getFileSize(const std::string &filePath);

GetRequest::GetRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, GET)
{
	uint16_t	targetType;

	if (this->_code != 0)
		return ;
	targetType = isDirOrFile(this->_url);
	if (targetType == DIRE)
		directoryCase(*this);
	else if (targetType == LS_FILE)
	{
		setResponse(HTTP_OK);
		openFileAndSetSize();
	}
	else
		setResponse(targetType);
}

GetRequest::~GetRequest()
{
}

void	GetRequest::openFileAndSetSize(void)
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

void	GetRequest::setResponseWithAutoIndex(uint16_t code, const std::string &autoIndexPage)
{
	this->_code = code;
	this->_autoIndexPage = autoIndexPage;
}
