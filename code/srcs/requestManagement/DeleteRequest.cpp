#include <stdint.h>           // for uint16_t
#include <string>             // for string, basic_string

#include "ARequestType.hpp"   // for ARequestType, DIRE, LS_FILE
#include "DeleteRequest.hpp"  // for DeleteRequest
#include "EMethods.hpp"       // for EMethods

class RequestContext;
class ServerConfiguration;  // lines 9-9

uint16_t	isDirOrFile(const std::string& path);
uint16_t	directoryCase(const std::string &path, DeleteRequest &del);
uint16_t	fileCase(const std::string &path, DeleteRequest &del);

DeleteRequest::DeleteRequest
(
	std::string url,
	const ServerConfiguration &config,
	const std::string& domain,
	RequestContext& requestContext
) :
	ARequestType(url, config, DELETE, domain, requestContext)
{
	uint16_t	fileType;

	if (this->_code != 0)
		return ;
	fileType = isDirOrFile(this->_path);
	if (fileType == DIRE)
		directoryCase(this->_path, *this);
	else if (fileType == LS_FILE)
		fileCase(this->_path, *this);
	else
		setResponse(fileType);
}

DeleteRequest::~DeleteRequest()
{
}
