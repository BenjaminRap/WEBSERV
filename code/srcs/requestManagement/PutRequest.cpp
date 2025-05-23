#include <fcntl.h>                // for O_CREAT, O_EXCL, O_WRONLY
#include <stdint.h>               // for uint16_t
#include <cstring>                // for size_t
#include <exception>              // for exception
#include <string>                 // for basic_string, string

#include "ARequestType.hpp"       // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"           // for EMethods
#include "FileFd.hpp"             // for FileFd
#include "PutRequest.hpp"         // for PutRequest
#include "SharedResource.hpp"     // for freePointer, SharedResource
#include "requestStatusCode.hpp"  // for HTTP_FORBIDDEN, HTTP_CONFLICT, HTTP...

class EPollHandler;
class ServerConfiguration;  // lines 14-14

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

uint16_t	getStatusCodeFromErrno(int errnoValue);

PutRequest::PutRequest
(
	std::string url,
	const ServerConfiguration &config,
	EPollHandler& ePollHandler,
	const std::string& domain
) :
	ARequestType(url, config, ePollHandler, PUT, domain)
{
	std::string path;
	uint16_t	fileType;

	if (this->_code != 0)
		return ;
	this->_fileName = getName(this->_url);
	path = this->_url;
	removeFileName(this->_url);
	fileType = isDirOrFile(path);
	if (fileType == DIRE)
		this->setResponse(HTTP_CONFLICT);
	else if (this->_fileName.empty() && fileType == HTTP_NOT_FOUND)
		this->setResponse(HTTP_CONFLICT);
	else if (!canWrite(this->_url) && fileType != HTTP_FORBIDDEN)
		this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
	else
	{
		try
		{
			FileFd*	fileFd = new FileFd(path, O_CREAT | O_EXCL | O_WRONLY, 0666);

			this->_inFd.setManagedResource(fileFd, freePointer);
			this->setResponse(HTTP_CREATED);
		}
		catch(const FileFd::FileOpeningError& openError)
		{
			const uint16_t	code = getStatusCodeFromErrno(openError.getErrno());

			this->setResponse(code);
		}
	}
}

PutRequest::~PutRequest()
{
}
