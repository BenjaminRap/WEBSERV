#include <stddef.h>     			// for size_t
#include <sys/types.h>  			// for ssize_t

#include "ABody.hpp"    			// for ABody
#include "requestStatusCode.hpp"	// for HTTP_OK

ABody::ABody(int fd) :
	_fd(fd),
	_finished(false),
	_status(HTTP_OK)
{
}

ABody::~ABody()
{
}

int	ABody::getFd() const
{
	return (_fd);
}

void	ABody::setFinished(uint16_t status)
{
	_finished = true;
	_status = status;
}

bool	ABody::getFinished() const
{
	return (_finished);
}

uint16_t	ABody::getStatus() const
{
	return (_status);
}

ssize_t	ABody::callInstanceWriteToFd(ABody &body, const void *buffer, size_t bufferCapacity)
{
	return (body.writeToFd(buffer, bufferCapacity));
}
