// IWYU pragma: private, include "FlowBuffer.hpp"

#include "FlowBuffer.hpp"

/**
 * @brief Redirect the data from srcFd to destFd.
 * @param srcFd The file descriptor in which the function will read or receive
 * the data.
 * @param readData Data that will be sent to the read custom function.
 * @param destFd The file descriptor in which the function will write or send
 * the data read from srcFd.
 * @param writeData Data that will be sent to the write function.
 * @param customRead The function that will read the data in the srcFd. If no parameter
 * is entered, the default customRead is readFromFdWithType and readData is a FdType.
 * @param customWrite The function that will write the data in the destFd. If no parameter
 * is entered, the default customWrite is writeToFdWithType and writeData is a FdType.
 * @return Return FLOW_ERROR on error, FLOW_DONE if there is nothing more to read/write
 * and FLOW_MORE if there is more to read/write. In the latter case, we should
 * wait for an EPOLLIN/EPOLLOUT event before calling this function again.
 */
// template <typename ReadData, typename WriteData>
// FlowState	FlowBuffer::redirectContent
// (
// 	int srcFd,
// 	ReadData &readData,
// 	int destFd,
// 	WriteData &writeData,
// 	ssize_t (&customRead)(int fd, char *buffer, size_t bufferCapacity, ReadData &readData),
// 	ssize_t (&customWrite)(int fd, char *buffer, size_t bufferCapacity, WriteData &writeData)
// )
// {
// 	const FlowState	readState = redirectFdContentToBuffer(srcFd, readData, customRead);
//
//
// 	if (readState == FLOW_ERROR)
// 		return (FLOW_ERROR);
// 	const FlowState writeState = redirectBufferContentToFd(destFd, writeData, customWrite);
//
// 	if (writeState == FLOW_DONE)
// 		return ((readState == FLOW_BUFFER_FULL) ? FLOW_MORE : readState);
// 	return (writeState);
// }

/**
 * @brief Redirect the data from this instance's buffer to destFd.
 * @param destFd The file descriptor in which the data from buffer will be written
 * or sent into.
 * @param writeData that will be sent to the write function.
 * @param customWrite The function that will write the data in the destFd. If no parameter
 * is entered, the default customWrite is writeToFdWithType and writeData is a FdType.
 * @return Return FLOW_ERROR on error, FLOW_DONE if there is nothing more to write
 * and FLOW_MORE if there is more to write. In the latter case, we should
 * wait for an EPOLLOUT event before calling this function again.
 */
template <typename WriteData>
FlowState	FlowBuffer::redirectBufferContentToFd
(
	int destFd,
	WriteData &writeData,
	ssize_t (&customWrite)(int fd, char *buffer, size_t bufferCapacity, WriteData &writeData)
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
	return (FLOW_MORE);
}

/**
 * @brief Read or recv all the data from srcFd and write it in the internal
 * buffer.
 * @param srcFd The fd this functions will read from.
 * @param readData Data that will be sent to the read custom function.
 * @param customRead The function that will read the data in the srcFd. If no parameter
 * is entered, the default customRead is readFromFdWithType and readData is a FdType.
 * @return Return FLOW_ERROR on error, FLOW_DONE if there is nothing more to read,
 * FLOW_BUFFER_FULL if the buffer is full and nothing could be read, and FLOW_MORE
 * if there is more to read. In the latter case, we should wait for an EPOLLIN
 * event before calling this function again.
 * BUFFER_FULL also means that there is more to read.
 */
template <typename ReadData>
FlowState	FlowBuffer::redirectFdContentToBuffer
(
	int srcFd,
	ReadData &readData,
	ssize_t (&customRead)(int fd, char *buffer, size_t bufferCapacity, ReadData &readData)
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
	return ((_bufferLength >= _bufferCapacity) ? FLOW_BUFFER_FULL : FLOW_MORE);
}
