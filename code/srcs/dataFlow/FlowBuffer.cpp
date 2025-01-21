#include <unistd.h>
#include <cerrno>
#include <sys/socket.h>

#include "FlowBuffer.hpp"

/************************Constructors / Destructors****************************/

FlowBuffer::FlowBuffer(char *buffer, size_t bufferCapacity) :
	_buffer(buffer),
	_bufferCapacity(bufferCapacity),
	_bufferLength(0),
	_numCharsWritten(0)
{

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
 * @return Return -1 on error, 0 if there is nothing more to read, and >0 if there
 * is more to read. In the later case, we need to wait for another EPOLLOUT/EPOLLIN
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
 * @return Return -1 on error, 0 if there is nothing more to read, and >0 if there
 * is more to read. In the later case, we need to wait for another EPOLLOUT/EPOLLIN
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
