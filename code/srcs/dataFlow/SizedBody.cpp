#include <stddef.h>       			// for size_t
#include <sys/types.h>    			// for ssize_t
#include <unistd.h>       			// for write
#include <algorithm>      			// for min

#include "ABody.hpp"      			// for ABody
#include "requestStatusCode.hpp"	// for HTTP_...
#include "socketCommunication.hpp"	// for checkError
#include "SizedBody.hpp"  			// for SizedBody

SizedBody::SizedBody(int fd, size_t size) :
	ABody(fd),
	_size(size),
	_numCharsWritten(0)
{
	if (_size == 0)
		setFinished(HTTP_OK);
}

SizedBody::~SizedBody()
{
}

ssize_t	SizedBody::writeToFd(const void *buffer, size_t bufferCapacity)
{
	if (getFinished())
		return (0);
	const size_t	numCharsToWrite = std::min(_size - _numCharsWritten, bufferCapacity);

	const ssize_t	written = writeOrIgnore(buffer, numCharsToWrite);
	
	if (checkError<ssize_t>(written, -1, "write() : "))
	{
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
		return (-1);
	}
	_numCharsWritten += written;
	if (_numCharsWritten >= _size)
		setFinished(HTTP_OK);
	return (written);
}
