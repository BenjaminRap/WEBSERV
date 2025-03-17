#include <unistd.h>

#include "Body.hpp"
#include "socketCommunication.hpp"

Body::Body(int fd, bool isBlocking) :
	_fd(fd),
	_isBlocking(isBlocking)
{
}

Body::~Body()
{
	checkError(close(_fd), -1, "close() : ");
}

int	Body::getFd() const
{
	return (_fd);
}

void	Body::setFinished()
{
	_finished = true;
}

bool	Body::getFinished() const
{
	return (_finished);
}


bool	Body::getIsBlocking() const
{
	return (_isBlocking);
}

ssize_t	Body::callInstanceWriteToFd(int fd, char *buffer, size_t bufferCapacity, Body &body)
{
	if (body.getFd() != fd)
		throw std::logic_error("Body writeFlowBufferToFd function called with the wrong fd !");
	return (body.writeToFd(buffer, bufferCapacity));
}
