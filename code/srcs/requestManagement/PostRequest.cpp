#include <string>                 // for basic_string, string

#include "ARequestType.hpp"       // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"           // for EMethods
#include "FileFd.hpp"             // for FileFd
#include "PostRequest.hpp"         // for PostRequest
#include "requestStatusCode.hpp"  // for HTTP_CONFLICT, HTTP_CREATED, HTTP_F...

class RequestContext;
class ServerConfiguration;

uint16_t	isDirOrFile(const std::string& path);
std::string getName(std::string &path);
void	removeFileName(std::string &url);

PostRequest::PostRequest
		(
				std::string url,
				const ServerConfiguration &config,
				const std::string& domain,
				RequestContext& requestContext
		) :
		ARequestType(url, config, POST, domain, requestContext)
{
	if (this->_code == 0)
	{
		uint16_t	fileType;

		fileType = isDirOrFile(this->_path);
		if (fileType == HTTP_NOT_FOUND)
			setResponse(HTTP_NOT_FOUND);
		else if (fileType == HTTP_FORBIDDEN)
			setResponse(HTTP_FORBIDDEN);
		else
			setResponse(HTTP_METHOD_NOT_ALLOWED);
	}

}

PostRequest::~PostRequest()
{
}
