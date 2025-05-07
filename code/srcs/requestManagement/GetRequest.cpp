#include <fcntl.h>                // for O_RDONLY
#include <stdint.h>               // for uint16_t
#include <sys/types.h>            // for ssize_t
#include <exception>              // for exception
#include <string>                 // for string, basic_string

#include "ARequestType.hpp"       // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"           // for EMethods
#include "FileFd.hpp"             // for FileFd
#include "GetRequest.hpp"         // for GetRequest
#include "SharedResource.hpp"     // for freePointer, SharedResource
#include "requestStatusCode.hpp"  // for HTTP_INTERNAL_SERVER_ERROR, HTTP_OK

class EPollHandler;
class ServerConfiguration;  // lines 14-14

uint16_t	isDirOrFile(const std::string& path);
void		directoryCase(GetRequest& get);
ssize_t		getFileSize(const std::string &filePath);

GetRequest::GetRequest
(
	std::string url,
	const ServerConfiguration &config,
	EPollHandler& ePollHandler,
	const std::string& domain
) :
	ARequestType(url, config, ePollHandler, GET, domain)
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
	try
	{
		FileFd*	fileFd = new FileFd(_url, O_RDONLY);

		this->_outFd.setManagedResource(fileFd, freePointer);
	}
	catch(std::exception& exception)
	{
		this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
		return ;
	}
}

void	GetRequest::setResponseWithAutoIndex(uint16_t code, const std::string &autoIndexPage)
{
	this->_code = code;
	this->_autoIndexPage = autoIndexPage;
}
