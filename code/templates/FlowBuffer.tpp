#include "FlowBuffer.hpp"

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
 * FLOW_EAGAIN_RECV if there is more to receive and we need to wait an EPOLLIN event
 * and FLOW_EAGAIN_SEND of there is more to send and we need to wait for an EPOLLOUT event.
 */
template <typename Data>
FlowState	FlowBuffer::redirectContent
(
	int srcFd,
	Data &readData,
	int destFd,
	Data &writeData,
	ssize_t (&customRead)(int fd, char *buffer, size_t bufferCapacity, Data &data),
	ssize_t (&customWrite)(int fd, char *buffer, size_t bufferCapacity, Data &data)
)
{
	ssize_t			rd;

	do
	{
		const FlowState	flowState = redirectContentFromBuffer(destFd, writeData, customWrite);

		if (flowState != FLOW_DONE)
			return (flowState);
		rd = customRead(srcFd, _buffer, _bufferCapacity, readData);
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
 * and FLOW_EAGAIN_SEND if there is more to send. In the later case, we need to wait for
 * another EPOLLOUT before calling this function again, until it returns FLOW_DONE
 * or FLOW_ERROR.
 */
template <typename Data>
FlowState	FlowBuffer::redirectContentFromBuffer
(
	int destFd,
	Data &writeData,
	ssize_t (&customWrite)(int fd, char *buffer, size_t bufferCapacity, Data &data)
)
{
	size_t	numCharsToWrite;

	numCharsToWrite = _bufferLength - _numCharsWritten;
	while (_numCharsWritten < _bufferLength)
	{
		const ssize_t written = customWrite(destFd, _buffer + _numCharsWritten, numCharsToWrite, writeData);
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
 * has closed the connection, FLOW_EAGAIN_RECV if there is more to receive and
 * FLOW_BUFFER_FULL if the buffer is full. If the buffer is full and the client
 * has closed the connection, this function will return FLOW_BUFFER_FULL.
 * In the case of return FLOW_EAGAIN_RECV, we need to wait for another EPOLLIN event
 * and call this function again.
 */
template <typename Data>
FlowState	FlowBuffer::redirectContentToBuffer
(
	int srcFd,
	Data &readData,
	ssize_t (&customRead)(int fd, char *buffer, size_t bufferCapacity, Data &data)
)
{
	size_t	remainingCapacity;

	remainingCapacity = _bufferCapacity - _bufferLength;
	while (_bufferLength < _bufferCapacity)
	{
		const ssize_t rd = customRead(srcFd, _buffer + _bufferLength, remainingCapacity, readData);
		if (rd == -1)
			return (FLOW_ERROR);
		if (rd == 0)
			return (FLOW_DONE);
		remainingCapacity -= rd;
		_bufferLength += rd;
	}
	return (FLOW_BUFFER_FULL);
}