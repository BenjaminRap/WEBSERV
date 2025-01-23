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
 * @return Return FLOW_ERROR on error, FLOW_DONE if there is nothing more to read/write,
 * FLOW_MORE_RECV if there is more to receive and we need to wait an EPOLLIN event
 * and FLOW_MORE_SEND of there is more to send and we need to wait for an EPOLLOUT event.
 */
FlowState	FlowBuffer::redirectContent(int srcFd, FdType srcType, int destFd, FdType destType)
{
	ssize_t			rd;

	do
	{
		const FlowState	flowState = redirectContentFromBuffer(destFd, destType);

		if (flowState != FLOW_DONE)
			return (flowState);
		if (srcType == SOCKETFD)
		{
			rd = recv(srcFd, _buffer, _bufferCapacity, MSG_DONTWAIT | MSG_NOSIGNAL);
			if (rd == -1 && errno == EAGAIN)
				return (FLOW_MORE_RECV);
		}
		else
			rd = read(srcFd, _buffer, _bufferCapacity);
		if (rd == -1)
			return (FLOW_ERROR);
		_numCharsWritten = 0;
		_bufferLength = rd;
	}
	while (rd != 0);
	return (FLOW_DONE);
}

/**
 * @brief Redirect the data from this instance's buffer to destFd.
 * @param destFd The file descriptor in which the data from buffer will be written
 * or sent into.
 * @param destType The type of destFd, either FILEFD or SOCKETFD, it will determines
 * if the function uses write or recv.
 * @return Return FLOW_ERROR on error, FLOW_DONE if there is nothing more to write,
 * and FLOW_MORE_SEND if there is more to send. In the later case, we need to wait for
 * another EPOLLOUT before calling this function again, until it returns FLOW_DONE
 * or FLOW_ERROR.
 */
FlowState	FlowBuffer::redirectContentFromBuffer(int destFd, FdType destType)
{
	size_t	numCharsToWrite;

	numCharsToWrite = _bufferLength - _numCharsWritten;
	while (_numCharsWritten < _bufferLength)
	{
		ssize_t	written;

		if (destType == SOCKETFD)
		{
			written = send(destFd, _buffer + _numCharsWritten, numCharsToWrite, MSG_DONTWAIT | MSG_NOSIGNAL);
			if (written == -1 && errno == EAGAIN)
				return (FLOW_MORE_SEND);
		}
		else
			written = write(destFd, _buffer + _numCharsWritten, numCharsToWrite);
		if (written == -1)
			return (FLOW_ERROR);
		_numCharsWritten += written;
		numCharsToWrite -= written;
	}
	_bufferLength = 0;
	_numCharsWritten = 0;
	return (FLOW_DONE);
}

/**
 * @brief Read or recv all the data from srcFd and write it in the internal
 * buffer.
 * @param srcFd The fd this functions will read from.
 * @param srcType The type of srcFd, either SOCKETFD or FILEFD, it will determine
 * if the function uses read or recv.
 * @return This function returns FLOW_ERROR on error, FLOW_DONE if the client
 * has closed the connection, FLOW_MORE_RECV if there is more to receive and
 * FLOW_BUFFER_FULL if the buffer is full. If the buffer is full and the client
 * has closed the connection, this function will return FLOW_BUFFER_FULL.
 * In the case of return FLOW_MORE_RECV, we need to wait for another EPOLLIN event
 * and call this function again.
 */
FlowState	FlowBuffer::redirectContentToBuffer(int srcFd, FdType srcType)
{
	size_t	remainingCapacity;
	ssize_t	rd;

	remainingCapacity = _bufferCapacity - _bufferLength;
	while (_bufferLength < _bufferCapacity)
	{
		if (srcType == SOCKETFD)
		{
			rd = recv(srcFd, _buffer + _bufferLength, remainingCapacity, MSG_DONTWAIT | MSG_NOSIGNAL);
			if (rd == -1 && errno == EAGAIN)
				return (FLOW_MORE_RECV);
		}
		else
			rd = read(srcFd, _buffer + _bufferLength, remainingCapacity);
		if (rd == -1)
			return (FLOW_ERROR);
		if (rd == 0)
			return (FLOW_DONE);
		remainingCapacity -= rd;
		_bufferLength += rd;
	}
	return (FLOW_BUFFER_FULL);
}

size_t		FlowBuffer::getBufferLength(void) const
{
	return (_bufferLength);
}


size_t		FlowBuffer::getNumCharsWritten(void) const
{
	return (_numCharsWritten);
}