#include <stddef.h>               // for size_t
#include <stdint.h>               // for uint16_t
#include <sys/types.h>            // for ssize_t
#include <unistd.h>               // for write
#include <stdexcept>              // for logic_error
#include <string>                 // for basic_string

#include "ABody.hpp"              // for ABody, ABodyChilds
#include "ChunkedBody.hpp"        // for ChunkedBody
#include "SizedBody.hpp"          // for SizedBody
#include "requestStatusCode.hpp"  // for HTTP_OK

ABody::ABody(int fd, ABodyChilds type) :
	_fd(fd),
	_finished(false),
	_status(HTTP_OK),
	_written(0),
	_type(type)
{
}

ABody::ABody(ABodyChilds type) :
	_fd(-1),
	_finished(false),
	_status(HTTP_OK),
	_written(0),
	_type(type)
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

ABodyChilds	ABody::getType(void) const
{
	return (_type);
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
	if (_fd > 0)
	{
		const ssize_t	written = write(_fd, buffer, bufferCapacity);

		if (written != -1)
			_written += written;
		return (written);
	}
	_written += bufferCapacity;
	return (bufferCapacity);
}

ssize_t	ABody::writeToFd(ABody &body, const void *buffer, size_t bufferCapacity)
{
	switch (body.getType()) {
		case SIZED_BODY:
		{
			SizedBody*	sizedBody = static_cast<SizedBody*>(&body);

			return (sizedBody->writeToFd(buffer, bufferCapacity));
		}
		case CHUNKED_REQUEST:
		{
			ChunkedBody*	chunkedBody = static_cast<ChunkedBody*>(&body);

			return (chunkedBody->writeToFd(buffer, bufferCapacity));
		}
		default:
		{
			throw std::logic_error("A ABody type isn't take into account in the writeToFd static method !");
		}
	}
}
