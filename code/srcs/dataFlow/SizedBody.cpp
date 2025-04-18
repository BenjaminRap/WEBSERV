#include <stddef.h>       			// for size_t
#include <sys/types.h>    			// for ssize_t
#include <unistd.h>       			// for write
#include <algorithm>      			// for min
#include <iostream>       			// for char_traits, basic_ostream, operator<<, cerr

#include "ABody.hpp"      			// for ABody
#include "socketCommunication.hpp"	// for checkError
#include "SizedBody.hpp"  			// for SizedBody

SizedBody::SizedBody(int fd, size_t size) :
	ABody(fd),
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
	const size_t	numCharsToWrite = std::min(_size - _numCharsWritten, bufferCapacity);

	const ssize_t	written = (getFd() == -1) ?
		numCharsToWrite
		: write(getFd(), buffer, numCharsToWrite);
	
	if (checkError<ssize_t>(written, -1, "write() : "))
		return (-1);
	_numCharsWritten += written;
	if (_numCharsWritten == _size)
		setFinished();
	else if (_numCharsWritten > _size)
		std::cerr << "Logic Error: addCharsWritten() : _numCharsWritten superior to _size" << std::endl;	
	return (((size_t)written != numCharsToWrite) ? -1 : written);
}
