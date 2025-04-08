#include <stdint.h>           // for uint16_t
#include <string>             // for string, basic_string

#include "ARequestType.hpp"   // for ARequestType, DIRE, LS_FILE
#include "DeleteRequest.hpp"  // for DeleteRequest
#include "EMethods.hpp"       // for EMethods

class ServerConfiguration;  // lines 6-6

uint16_t	isDirOrFile(const std::string& path);
uint16_t	directoryCase(const std::string &path, DeleteRequest &del);
uint16_t	fileCase(const std::string &path, DeleteRequest &del);

DeleteRequest::DeleteRequest(std::string url, const std::string &domain, const ServerConfiguration &config) : ARequestType(url, config, DELETE, domain)
{
	uint16_t	fileType;

	if (this->_code != 0)
		return ;
	fileType = isDirOrFile(this->_url);
	if (fileType == DIRE)
		directoryCase(this->_url, *this);
	else if (fileType == LS_FILE)
		fileCase(this->_url, *this);
	else
		setResponse(fileType);
}

DeleteRequest::~DeleteRequest()
{
}
