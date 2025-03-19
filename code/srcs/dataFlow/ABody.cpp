#include <stddef.h>                 // for size_t
#include <sys/types.h>              // for ssize_t
#include <unistd.h>                 // for close

#include "ABody.hpp"                // for ABody

ABody::ABody(int fd, bool isBlocking) :
	_fd(fd),
	_isBlocking(isBlocking)
{
}

ABody::~ABody()
{
}

int	ABody::getFd() const
{
	return (_fd);
}

void	ABody::setFinished()
{
	_finished = true;
}

bool	ABody::getFinished() const
{
	return (_finished);
}


bool	ABody::getIsBlocking() const
{
	return (_isBlocking);
}

ssize_t	ABody::callInstanceWriteToFd(ABody &body, const void *buffer, size_t bufferCapacity)
{
	return (body.writeToFd(buffer, bufferCapacity));
}
