#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>
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
		throw std::logic_error("The buffer passed as argument is empty");
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

/**
 * @brief Redirect the data from srcFd to destFd.
 * @param srcFd The file descriptor in which the function will read or receive
 * the data.
 * @param srcType The type of srcFd, either SOCKETFD or FILEFD, it will determines
 * if the function uses read or recv.
 * @param destFd The file descriptor in which the function will write or send
 * the data read from srcFd.
 * @param destType The type of destFd, either SOCKETFD or FILEFD, it will determines
 * if the function uses write or send.
 * @return Return -1 on error, 0 if there is nothing more to read/write, and >0 if there
 * is more to read/write. In the later case, we need to wait for another EPOLLOUT/EPOLLIN
 * before calling this function again, until it returns 0 or -1.
 */
ssize_t	FlowBuffer::redirectContent(int srcFd, FdType srcType, int destFd, FdType destType)
{
	ssize_t			rd;

	do
	{
		const ssize_t	numCharsToWrite = redirectContentFromBuffer(destFd, destType);

		if (numCharsToWrite != 0)
			return (numCharsToWrite);
		if (srcType == SOCKETFD)
			rd = recv(srcFd, _buffer, _bufferCapacity, MSG_DONTWAIT | MSG_NOSIGNAL);
		else
			rd = read(srcFd, _buffer, _bufferCapacity);
		if (rd == -1)
			return ((errno == EAGAIN) ? 1 : -1);
		_numCharsWritten = 0;
		_bufferLength = rd;
	}
	while (rd != 0);
	return (0);
}

/**
 * @brief Redirect the data from this instance's buffer to destFd.
 * @param destFd The file descriptor in which the data from buffer will be written
 * or sent into.
 * @param destType The type of destFd, either FILEFD or SOCKETFD, it will determines
 * if the function uses write or recv.
 * @return Return -1 on error, 0 if there is nothing more to write, and >0 if there
 * is more to write. In the later case, we need to wait for another EPOLLOUT
 * before calling this function again, until it returns 0 or -1.
 */
ssize_t	FlowBuffer::redirectContentFromBuffer(int destFd, FdType destType)
{
	size_t	numCharsToWrite;

	numCharsToWrite = _bufferLength - _numCharsWritten;
	while (numCharsToWrite != 0)
	{
		ssize_t	written;

		if (destType == SOCKETFD)
			written = send(destFd, _buffer + _numCharsWritten, numCharsToWrite, MSG_DONTWAIT | MSG_NOSIGNAL);
		else
			written = write(destFd, _buffer + _numCharsWritten, numCharsToWrite);
		if (written == -1)
			return ((errno == EAGAIN) ? _numCharsWritten : -1);
		_numCharsWritten += written;
		numCharsToWrite -= written;
	}
	return (0);
}
