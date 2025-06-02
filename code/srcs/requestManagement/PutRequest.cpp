#include <fcntl.h>                // for O_CREAT, O_WRONLY
#include <stdint.h>               // for uint16_t
#include <cstring>                // for size_t
#include <string>                 // for basic_string, string

#include "ARequestType.hpp"       // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"           // for EMethods
#include "FileFd.hpp"             // for FileFd
#include "PutRequest.hpp"         // for PutRequest
#include "SharedResource.hpp"     // for freePointer, SharedResource
#include "requestStatusCode.hpp"  // for HTTP_CONFLICT, HTTP_CREATED, HTTP_F...

class RequestContext;
class ServerConfiguration;  // lines 14-14

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
	const std::string& domain,
	RequestContext& requestContext
) :
	ARequestType(url, config, PUT, domain, requestContext),
	_fileName()
{
	std::string path;

	if (this->_code != 0)
		return ;
	this->_fileName = getName(this->_path);
	path = this->_path;
	removeFileName(this->_path);
	if (_targetType == DIRE)
		this->setResponse(HTTP_CONFLICT);
	else if (this->_fileName.empty() && _targetType == HTTP_NOT_FOUND)
		this->setResponse(HTTP_CONFLICT);
	else if (!canWrite(this->_path) && _targetType != HTTP_FORBIDDEN)
		this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
	else
	{
		try
		{
			FileFd*	fileFd = new FileFd(path.c_str(), O_CREAT | O_WRONLY | O_TRUNC, 0666);

			this->_inFd.setManagedResource(fileFd, freePointer);
			if (_targetType == LS_FILE)
				this->setResponse(HTTP_NO_CONTENT);
			else
				this->setResponseWithLocation(HTTP_CREATED, this->_path, false);
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
