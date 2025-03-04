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


std::string getName(std::string path)
{
	size_t	pos;

	pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return (path);
	return (path.erase(0, pos + 1));
}

void	removeFileName(std::string &url)
{
	size_t	pos;

	pos = url.find_last_of('/');
	if (pos == std::string::npos)
		return ;
	url.erase(pos + 1);
}


PutRequest::PutRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, PUT), _fd(-1)
{
	std::string path;
	int			ret;

	if (this->_code != 0)
		return ;
	this->_fileName = getName(this->_url);
	path = this->_url;
	removeFileName(this->_url);
	ret = isDirOrFile(path);
	if ((ret != NF && ret != FORBIDEN) || (this->_fileName.empty() && ret == NF))
		this->setResponse(409, "Conflict", "Conflict");
	else if (!canWrite( this->_url) && ret != FORBIDEN)
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
