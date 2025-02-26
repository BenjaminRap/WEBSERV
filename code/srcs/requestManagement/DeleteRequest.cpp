#include "DeleteRequest.hpp"

int							isDirOrFile(const std::string& path);
int							directoryCase(const std::string &path, DeleteRequest &del);
int							fileCase(const std::string &path, DeleteRequest &del);

DeleteRequest::DeleteRequest(std::string url, const ServerConfiguration &config) : ARequest(url, config, DELETE)
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
		this->setResponse(403, "Forbidden", "Forbidden");
	else
		setResponse(404, "Not Found", config.getErrorPage(404));
}

DeleteRequest::~DeleteRequest()
{
}
