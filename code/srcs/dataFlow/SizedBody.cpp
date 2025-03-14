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

size_t	SizedBody::getSize() const
{
	return (_size);
}

void	SizedBody::addCharsWritten(size_t add)
{
	_numCharsWritten += add;
	if (_numCharsWritten == _size)
		setFinished();
	else if (_numCharsWritten > _size)
		std::cerr << "Logic Error: addCharsWritten() : _numCharsWritten superior to _size" << std::endl;	
}

ssize_t	SizedBody::writeToFile(int fd, char *buffer, size_t bufferCapacity, SizedBody &sizedBody)
{
	const size_t	numCharsToWrite = std::min(sizedBody.getSize(), bufferCapacity);
	const ssize_t	written = write(fd, buffer, numCharsToWrite);
	
	if (written == -1)
		return (-1);
	sizedBody.addCharsWritten(written);
	if (written == -1)
		return (-1);
	return (((size_t)written != numCharsToWrite) ? -1 : written);
}

FlowState	SizedBody::writeBodyFromBufferToFile(FlowBuffer &flowBuffer)
{
	return (flowBuffer.redirectBufferContentToFd(getFd(), *this, SizedBody::writeToFile));
}

// FlowState	SizedBody::redirectBodyFromSocketToFile(FlowBuffer &flowBuffer, int socketFd)
// {
// 	FdType	socketType = SOCKETFD;
//
// 	return (flowBuffer.redirectContent(socketFd, socketType, getFd(), *this,
// 		readFromFdWithType,
// 		SizedBody::writeToFile));
// }
