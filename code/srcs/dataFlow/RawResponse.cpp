#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <cerrno>

#include "RawResponse.hpp"

static ssize_t	writeFirstPartToFd(int fd, const char *buffer, size_t numsCharsToWrite)
{
	while (numsCharsToWrite > 0)
	{
		const int		written = send(fd, buffer, numsCharsToWrite, MSG_DONTWAIT | MSG_NOSIGNAL);

		if (written == -1)
			return ((errno == EAGAIN) ? numsCharsToWrite : -1);
		buffer += written;
		numsCharsToWrite -= written;
	}
	return (0);
}

ssize_t	RawResponse::writeToFd(int fd)
{
	if (_numCharsToWrite > 0)
	{
		const size_t	numsCharsWritten = _firstPart.size() - _numCharsToWrite;

		_numCharsToWrite = writeFirstPartToFd(fd, _firstPart.c_str() + numsCharsWritten, _numCharsToWrite);
		if (_numCharsToWrite != 0)
			return (_numCharsToWrite);
	}
	return (0);
}