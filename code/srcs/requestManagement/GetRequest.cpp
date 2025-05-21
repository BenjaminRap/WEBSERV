#include <fcntl.h>                // for O_RDONLY
#include <stdint.h>               // for uint16_t
#include <sys/types.h>            // for ssize_t
#include <string>                 // for string, basic_string

#include "ARequestType.hpp"       // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"           // for EMethods
#include "FileFd.hpp"             // for FileFd
#include "GetRequest.hpp"         // for GetRequest
#include "SharedResource.hpp"     // for freePointer, SharedResource
#include "requestStatusCode.hpp"  // for HTTP_OK

class RequestContext;
class ServerConfiguration;  // lines 15-15

uint16_t	isDirOrFile(const std::string& path);
void		directoryCase(GetRequest& get);
ssize_t		getFileSize(const std::string &filePath);

GetRequest::GetRequest
(
	std::string url,
	const ServerConfiguration &config,
	const std::string& domain,
	RequestContext& requestContext
) :
	ARequestType(url, config, GET, domain, requestContext)
{
	uint16_t	targetType;

	if (this->_code != 0)
		return ;
	targetType = isDirOrFile(this->_path);
	if (targetType == DIRE)
		directoryCase(*this);
	else if (targetType == LS_FILE)
	{
		setResponse(HTTP_OK);
		openFile();
	}
	else
		setResponse(targetType);
}

GetRequest::~GetRequest()
{
}

uint16_t	getStatusCodeFromErrno(int errnoValue);

void	GetRequest::openFile(void)
{
	try
	{
		FileFd*	fileFd = new FileFd(_path.c_str(), O_RDONLY);

		this->_outFd.setManagedResource(fileFd, freePointer);
	}
	catch(const FileFd::FileOpeningError& openError)
	{
		const uint16_t	code = getStatusCodeFromErrno(openError.getErrno());

		this->setResponse(code);
		return ;
	}
}

void	GetRequest::setResponseWithAutoIndex(uint16_t code, const std::string &autoIndexPage)
{
	this->_code = code;
	this->_autoIndexPage = autoIndexPage;
}
