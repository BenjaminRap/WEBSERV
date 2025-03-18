#include "PutRequest.hpp"
#include "socketCommunication.hpp"
#include "requestStatusCode.hpp"

int							isDirOrFile(const std::string& path);
bool						canWrite(const std::string &path);

std::string getName(std::string &path)
{
	size_t	pos;

	pos = path.find_last_of('/');
	if (pos == std::string::npos)
		return (path.substr());
	return (path.substr(pos + 1));
}

void	removeFileName(std::string &url)
{
	size_t	pos;

	pos = url.find_last_of('/');
	if (pos == std::string::npos)
		return ;
	url.erase(pos + 1);
}


PutRequest::PutRequest(std::string url, const ServerConfiguration &config) : ARequestType(url, config, PUT)
{
	std::string path;
	int			ret;

	if (this->_code != 0)
		return ;
	this->_fileName = getName(this->_url);
	path = this->_url;
	removeFileName(this->_url);
	ret = isDirOrFile(path);
	if ((ret != NF && ret != HTTP_FORBIDDEN) || (this->_fileName.empty() && ret == NF))
		this->setResponse(HTTP_CONFLICT);
	else if (!canWrite( this->_url) && ret != HTTP_FORBIDDEN)
		this->setResponse(HTTP_FORBIDDEN);
	else
	{
		this->_fd = open(path.c_str(), O_CREAT | O_EXCL, 0666);
		if (this->_fd == -1
			|| checkError(fcntl(this->_fd, F_SETFL, O_NONBLOCK | FD_CLOEXEC), -1, "fcntl() : ") == -1)
		{
			this->setResponse(HTTP_INTERNAL_SERVER_ERROR);
		}
		else
			this->setResponse(HTTP_CREATED);
	}
}

PutRequest::~PutRequest()
{
}
