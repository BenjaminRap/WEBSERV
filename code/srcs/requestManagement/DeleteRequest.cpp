#include "DeleteRequest.hpp"
#include "requestStatusCode.hpp"

int							isDirOrFile(const std::string& path);
int							directoryCase(const std::string &path, DeleteRequest &del);
int							fileCase(const std::string &path, DeleteRequest &del);

DeleteRequest::DeleteRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, DELETE)
{
	int			temp;

	if (this->_code != 0)
		return ;
	temp = isDirOrFile(this->_url);
	if (temp == DIRE)
		directoryCase(this->_url, *this);
	else if (temp == LS_FILE)
		fileCase(this->_url, *this);
	else if (temp == -1)
		this->setResponse(HTTP_FORBIDDEN);
	else
		setResponse(HTTP_NOT_FOUND);
}

DeleteRequest::~DeleteRequest()
{
}
