#include <stddef.h>     			// for size_t
#include <stdexcept>				// for std::logic_error
#include <sys/types.h>  			// for ssize_t
#include <unistd.h>					// for write

#include "ABody.hpp"    			// for ABody
#include "requestStatusCode.hpp"	// for HTTP_OK

ABody::ABody(int fd) :
	_fd(fd),
	_finished(false),
	_status(HTTP_OK)
{
}

ABody::ABody() :
	_fd(-1),
	_finished(false),
	_status(HTTP_OK)
{
}

ABody::~ABody()
{
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

size_t	ABody::getWritten(void) const
{
	return (_written);
}

void	ABody::setFd(int fd)
{
	if (!_finished && _written != 0)
		throw std::logic_error("Tried to change the fd of a body that has began to write data but not finished !");

	_finished = false;
	_written = 0;
	_fd = fd;
}

ssize_t	ABody::writeOrIgnore(const void* buffer, size_t bufferCapacity)
{
	if (_fd < 0)
	{
		const ssize_t	written = write(_fd, buffer, bufferCapacity);

		if (written != -1)
			_written += written;
		return (written);
	}
	return (bufferCapacity);
}

ssize_t	ABody::writeToFd(ABody &body, const void *buffer, size_t bufferCapacity)
{
	return (body.writeToFd(buffer, bufferCapacity));
}
