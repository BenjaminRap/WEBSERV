#include <algorithm>
#include <unistd.h>
#include <iostream>

#include "SizedBody.hpp"
#include "FlowBuffer.hpp"

SizedBody::SizedBody(int fd, size_t size, bool isBlocking) :
	Body(fd, isBlocking),
	_size(size),
	_numCharsWritten(0)
{
	if (_size == 0)
		setFinished();
}

SizedBody::~SizedBody()
{

}

ssize_t	SizedBody::writeToFd(const void *buffer, size_t bufferCapacity)
{
	const size_t	numCharsToWrite = std::min(_size, bufferCapacity);
	const ssize_t	written = write(getFd(), buffer, numCharsToWrite);
	
	if (written == -1)
		return (-1);
	_numCharsWritten += written;
	if (_numCharsWritten == _size)
		setFinished();
	else if (_numCharsWritten > _size)
		std::cerr << "Logic Error: addCharsWritten() : _numCharsWritten superior to _size" << std::endl;	
	return (((size_t)written != numCharsToWrite) ? -1 : written);
}
