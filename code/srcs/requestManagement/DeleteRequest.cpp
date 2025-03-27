#include "DeleteRequest.hpp"
#include "EMethods.hpp"           // for EMethods

class ServerConfiguration;

int							isDirOrFile(const std::string& path);
int							directoryCase(const std::string &path, DeleteRequest &del);
int							fileCase(const std::string &path, DeleteRequest &del);

DeleteRequest::DeleteRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, DELETE)
{
	int			fileType;

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
