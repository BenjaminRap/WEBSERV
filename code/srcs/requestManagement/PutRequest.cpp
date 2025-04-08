#include <fcntl.h>                  // for open, FD_CLOEXEC, O_CREAT, O_EXCL
#include <stdint.h>                 // for uint16_t
#include <cstring>                  // for size_t
#include <string>                   // for basic_string, string

#include "ARequestType.hpp"         // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"             // for EMethods
#include "PutRequest.hpp"           // for PutRequest
#include "SharedResource.hpp"       // for SharedResource
#include "requestStatusCode.hpp"    // for HTTP_FORBIDDEN, HTTP_INTERNAL_SER...
#include "socketCommunication.hpp"  // for addFlagsToFd, checkError, closeFd...

class ServerConfiguration;  // lines 11-11

uint16_t	isDirOrFile(const std::string& path);
bool		canWrite(const std::string &path);

std::string getName(std::string &path)
{
	size_t	pos;

	pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return (path.substr());
	return (path.substr(pos + 1));
}

void	removeFileName(std::string &url)
{
	size_t	pos;

	pos = url.find_last_of('/');
	if (pos == std::string::npos)
		return ;
	url.erase(pos + 1);
}


PutRequest::PutRequest(std::string url, const std::string &domain, const ServerConfiguration &config) : ARequestType(url, config, PUT, domain)
{
	std::string path;
	uint16_t	fileType;

	if (this->_code != 0)
		return ;
	this->_fileName = getName(this->_url);
	path = this->_url;
	removeFileName(this->_url);
	fileType = isDirOrFile(path);
	if ((fileType == DIRE || fileType == LS_FILE)
		|| (this->_fileName.empty() && fileType == HTTP_NOT_FOUND))
	{
		this->setResponse(HTTP_CONFLICT);
	}
	else if (!canWrite(this->_url) && fileType != HTTP_FORBIDDEN)
		this->setResponse(HTTP_FORBIDDEN);
	else
	{
		const int fd = open(path.c_str(), O_CREAT | O_EXCL | O_WRONLY, 0666);
		if (checkError(fd, -1, "open() : "))
		{
			this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
			return ;
		}
		this->_inFd.setManagedResource(fd, closeFdAndPrintError);
		if (addFlagsToFd(this->_inFd.getValue(), FD_CLOEXEC) == -1)
		{
			this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
			return ;
		}
		this->setResponse(HTTP_CREATED);
	}
}

PutRequest::~PutRequest()
{
}
