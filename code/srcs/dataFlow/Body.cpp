#include <unistd.h>

#include "Body.hpp"
#include "socketCommunication.hpp"

Body::Body(int fd) :
	_fd(fd)
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