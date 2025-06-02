#include <stdint.h>           // for uint16_t
#include <string>             // for string, basic_string

#include "ARequestType.hpp"   // for ARequestType, DIRE, LS_FILE
#include "DeleteRequest.hpp"  // for DeleteRequest
#include "EMethods.hpp"       // for EMethods

class RequestContext;
class ServerConfiguration;  // lines 9-9

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
	if (this->_code != 0)
		return ;
	if (_targetType == DIRE)
		directoryCase(this->_path, *this);
	else if (_targetType == LS_FILE)
		fileCase(this->_path, *this);
	else
		setResponse(_targetType);
}

DeleteRequest::~DeleteRequest()
{
}
