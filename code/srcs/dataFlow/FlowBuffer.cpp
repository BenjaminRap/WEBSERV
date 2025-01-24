#include <stdexcept>

#include "FlowBuffer.hpp"

/************************Constructors / Destructors****************************/

/**
 * @brief Create an instance of the FlowBuffer class.
 * @throw This function throw (std::logic_error) if bufferLength is superior to
 * bufferCapacity, if the buffer is null or if the bufferCapacity is set to 0.
 * @param buffer The buffer used to redirect the data.
 * @param bufferCapacity The maximum number of chars the buffer can store without
 * segfault.
 * @param bufferLength The number of chars that has already been written in the
 * buffer. If the index of a char is superior to bufferLength, its value is 
 * unkonwn and shouldn't be used.
 * If this constructor is called with a bufferLength superior to 0, the data
 * written in it will be handled.
 */
FlowBuffer::FlowBuffer(char *buffer, size_t bufferCapacity, size_t bufferLength) :
	_buffer(buffer),
	_bufferCapacity(bufferCapacity),
	_bufferLength(bufferLength),
	_numCharsWritten(0)
{
	if (bufferLength > bufferCapacity)
		throw std::logic_error("FlowBuffer constructor called with a bufferLength superior to the bufferCapacity");
	if (buffer == NULL)
		throw std::logic_error("The buffer passed as argument is NULL");
	if (bufferCapacity == 0)
		throw std::logic_error("The buffer capacity is 0");
}

FlowBuffer::FlowBuffer(const FlowBuffer& ref) :
	_buffer(ref._buffer),
	_bufferCapacity(ref._bufferCapacity),
	_bufferLength(ref._bufferLength),
	_numCharsWritten(ref._numCharsWritten)
{

}

FlowBuffer::~FlowBuffer()
{

}

/*****************************Member Functions*********************************/

size_t		FlowBuffer::getBufferLength(void) const
{
	return (_bufferLength);
}


size_t		FlowBuffer::getNumCharsWritten(void) const
{
	return (_numCharsWritten);
}

/************************FlowBuffer write/read functions***********************/

ssize_t	readFromFdWithType(int fd, char *buffer, size_t bufferCapacity, FdType &fdType)
{
	if (fdType == SOCKETFD)
		return (recv(fd, buffer, bufferCapacity, MSG_DONTWAIT | MSG_NOSIGNAL));
	return (read(fd, buffer, bufferCapacity));
}

ssize_t	writeToFdWithType(int fd, char *buffer, size_t bufferCapacity, FdType &fdType)
{
	if (fdType == SOCKETFD)
		return (send(fd, buffer, bufferCapacity, MSG_DONTWAIT | MSG_NOSIGNAL));
	return(write(fd, buffer, bufferCapacity));
}
