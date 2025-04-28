#include <stddef.h>     			// for size_t
#include <sys/types.h>  			// for ssize_t
#include <unistd.h>					// for write

#include "ABody.hpp"    			// for ABody
#include "requestStatusCode.hpp"	// for HTTP_OK

ABody::ABody(int fd) :
	_fd(fd),
	_file(NULL),
	_finished(false),
	_status(HTTP_OK)
{
}

ABody::ABody(std::FILE* file) :
	_fd(-1),
	_file(file),
	_finished(false),
	_status(HTTP_OK)
{
}

ABody::ABody() :
	_fd(-1),
	_file(NULL),
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

ssize_t	ABody::writeOrIgnore(const void* buffer, size_t bufferCapacity)
{
	if (_fd != -1)
		return (write(_fd, buffer, bufferCapacity));
	else if (_file != NULL)
		return (fwrite(buffer, 1, bufferCapacity, _file));
	return (bufferCapacity);
}

ssize_t	ABody::writeToFd(ABody &body, const void *buffer, size_t bufferCapacity)
{
	return (body.writeToFd(buffer, bufferCapacity));
}
