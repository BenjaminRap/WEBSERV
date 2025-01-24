#include "FlowBuffer.hpp"

/**
 * @brief Redirect the data from srcFd to destFd.
 * @param srcFd The file descriptor in which the function will read or receive
 * the data.
 * @param readData Data that will be sent to the read custom function.
 * @param destFd The file descriptor in which the function will write or send
 * the data read from srcFd.
 * @param writeData that will be sent to the write function.
 * @param customRead The function that will read the data in the srcFd. If no parameter
 * is entered, the defualt customRead is readFromFdWithType and readData is a FdType.
 * @param customWrite The function that will write the data in the destF. If no parameter
 * is entered, the defualt customWrite is writeToFdWithType and writeData is a FdType.
 * @return Return FLOW_ERROR on error, FLOW_DONE if there is nothing more to read/write,
 * FLOW_MORE_RECV if there is more to receive and we need to wait an EPOLLIN event
 * and FLOW_MORE_SEND of there is more to send and we need to wait for an EPOLLOUT event.
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
	const FlowState	flowState = redirectBufferContentToFd(destFd, writeData, customWrite);

	if (flowState != FLOW_DONE)
		return (flowState);
	const ssize_t rd = customRead(srcFd, _buffer, _bufferCapacity, readData);
	if (rd == -1)
		return (FLOW_ERROR);
	_numCharsWritten = 0;
	_bufferLength = rd;
	if (rd == 0)
		return (FLOW_DONE);
	return (FLOW_MORE_RECV);
}

/**
 * @brief Redirect the data from this instance's buffer to destFd.
 * @param destFd The file descriptor in which the data from buffer will be written
 * or sent into.
 * @param writeData that will be sent to the write function.
 * @param customWrite The function that will write the data in the destF. If no parameter
 * is entered, the defualt customWrite is writeToFdWithType and writeData is a FdType.
 * @return Return FLOW_ERROR on error, FLOW_DONE if there is nothing more to write,
 * and FLOW_MORE_SEND if there is more to send. In the later case, we need to wait for
 * another EPOLLOUT before calling this function again, until it returns FLOW_DONE
 * or FLOW_ERROR.
 */
template <typename Data>
FlowState	FlowBuffer::redirectBufferContentToFd
(
	int destFd,
	Data &writeData,
	ssize_t (&customWrite)(int fd, char *buffer, size_t bufferCapacity, Data &data)
)
{
	if (_numCharsWritten < _bufferLength)
	{
		const size_t	numCharsToWrite = _bufferLength - _numCharsWritten;
		const ssize_t	written = customWrite(destFd, _buffer + _numCharsWritten, numCharsToWrite, writeData);
		if (written == -1)
			return (FLOW_ERROR);
		_numCharsWritten += written;
	}
	if (_numCharsWritten >= _bufferLength)
	{
		_bufferLength = 0;
		_numCharsWritten = 0;
		return (FLOW_DONE);
	}
	return (FLOW_MORE_RECV);
}

/**
 * @brief Read or recv all the data from srcFd and write it in the internal
 * buffer.
 * @param srcFd The fd this functions will read from.
 * @param readData Data that will be sent to the read custom function.
 * @param customRead The function that will read the data in the srcFd. If no parameter
 * is entered, the defualt customRead is readFromFdWithType and readData is a FdType.
 * @return This function returns FLOW_ERROR on error, FLOW_DONE if the client
 * has closed the connection, FLOW_MORE_RECV if there is more to receive and
 * FLOW_BUFFER_FULL if the buffer is full. If the buffer is full and the client
 * has closed the connection, this function will return FLOW_BUFFER_FULL.
 * In the case of return FLOW_MORE_RECV, we need to wait for another EPOLLIN event
 * and call this function again.
 */
template <typename Data>
FlowState	FlowBuffer::redirectFdContentToBuffer
(
	int srcFd,
	Data &readData,
	ssize_t (&customRead)(int fd, char *buffer, size_t bufferCapacity, Data &data)
)
{
	size_t	remainingCapacity;

	if (_bufferLength >= _bufferCapacity)
		return (FLOW_BUFFER_FULL);
	remainingCapacity = _bufferCapacity - _bufferLength;
	const ssize_t rd = customRead(srcFd, _buffer + _bufferLength, remainingCapacity, readData);
	if (rd == -1)
		return (FLOW_ERROR);
	if (rd == 0)
		return (FLOW_DONE);
	_bufferLength += rd;
	return ((_bufferLength >= _bufferCapacity) ? FLOW_BUFFER_FULL : FLOW_MORE_RECV);
}