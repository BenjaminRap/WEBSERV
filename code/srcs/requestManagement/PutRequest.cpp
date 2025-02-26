#include "PutRequest.hpp"

int							isDirOrFile(const std::string& path);
bool						canWrite(const std::string &path);

bool	checkFileName(const std::string &fileName)
{
	if (fileName.empty())
		return (false);
	if (fileName.find('/') != std::string::npos)
		return (false);
	return (true);
}

PutRequest::PutRequest(std::string url, std::string &fileName, const ServerConfiguration &config) : ARequestType(url, config, PUT), _fd(-1)
{
	std::string path;

	if (this->_code != 0)
		return ;
	if (!checkFileName(fileName))
	{
		this->setResponse(400, "Bad Request", "Bad Request");
		return ;
	}
	path = this->_url + fileName;
	if (isDirOrFile(path) != NF)
		this->setResponse(409, "Conflict", "Conflict");
	else if (!canWrite(this->_url))
		this->setResponse(403, "Forbidden", "Forbidden");
	else
	{
		this->_fd = open(path.c_str(), O_CREAT | O_EXCL, 0666);
		if (this->_fd == -1)
			this->setResponse(500, "Internal Server Error", this->getError(500));
		else
			this->setResponse(201, "Created", path);
	}
}

PutRequest::~PutRequest()
{
}

void PutRequest::setFd(int fd)
{
	this->_fd = fd;
}

int PutRequest::getFd(void) const
{
	return (this->_fd);
}
