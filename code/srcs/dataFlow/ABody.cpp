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

ABody::ABody(int fd, ABodyChilds type, bool isFdBlocking) :
	_fd(fd),
	_isFdBlocking(isFdBlocking),
	_finished(false),
	_status(HTTP_OK),
	_written(0),
	_type(type),
	_hasWritten(false)
{
}

ABody::ABody(ABodyChilds type) :
	_fd(-1),
	_isFdBlocking(false),
	_finished(false),
	_status(HTTP_OK),
	_written(0),
	_type(type),
	_hasWritten(false)
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

ABody::ABodyChilds	ABody::getType(void) const
{
	return (_type);
}

bool	ABody::getHasWritten(void) const
{
	return (_hasWritten);
}

void	ABody::setFd(int fd, bool isFdBlocking)
{
	_fd = fd;
	if (fd < 0)
		_isFdBlocking = false;
	else
		_isFdBlocking = isFdBlocking;
}

ssize_t	ABody::writeOrIgnore(const void* buffer, size_t bufferCapacity)
{
	if (_fd > 0)
	{
		if (_isFdBlocking && _hasWritten)
			return (0);
		_hasWritten = true;

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
	body._hasWritten = false;
	switch (body.getType()) {
		case ABody::SIZED:
		{
			SizedBody*	sizedBody = static_cast<SizedBody*>(&body);

			return (sizedBody->writeToFd(buffer, bufferCapacity));
		}
		case ABody::CHUNKED:
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
