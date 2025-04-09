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
template <typename ReadData, typename WriteData>
FlowState	FlowBuffer::redirectContent
(
	ReadData readData,
	WriteData writeData,
	ssize_t (&customWrite)(WriteData writeData, const void *buffer, size_t bufferCapacity),
	ssize_t (&customRead)(ReadData readData, void *buffer, size_t bufferCapacity)
)
{
	const FlowState	readState = redirectFdContentToBuffer<ReadData>(readData, customRead);


	if (readState == FLOW_ERROR)
		return (FLOW_ERROR);
	const FlowState writeState = redirectBufferContentToFd<WriteData>(writeData, customWrite);

	if (writeState == FLOW_DONE)
		return (readState);
	return (writeState);
}

template <typename WriteData>
FlowState	FlowBuffer::redirectBufferContentToFd
(
	WriteData writeData,
	ssize_t (&customWrite)(WriteData writeData, const void *buffer, size_t bufferCapacity)
)
{
	if (_numCharsWritten < _contentLength)
	{
		const size_t	numCharsToWrite = _contentLength - _numCharsWritten;
		const ssize_t	written = customWrite(writeData, _buffer + _numCharsWritten, numCharsToWrite);
		if (written == -1)
			return (FLOW_ERROR);
		_numCharsWritten += written;
	}
	if (_numCharsWritten >= _contentLength)
	{
		_contentLength = 0;
		_numCharsWritten = 0;
		return (FLOW_DONE);
	}
	return (FLOW_MORE);
}

template <typename ReadData>
FlowState	FlowBuffer::redirectFdContentToBuffer
(
	ReadData readData,
	ssize_t (&customRead)(ReadData readData, void *buffer, size_t bufferCapacity)
)
{
	size_t	remainingCapacity;

	if (_numCharsWritten > MAX_CHARS_WRITTEN * _bufferCapacity)
		moveBufferContentToStart();
	else if (isBufferFull())
		return (FLOW_BUFFER_FULL);
	remainingCapacity = _bufferCapacity - _contentLength;
	const ssize_t rd = customRead(readData, _buffer + _contentLength, remainingCapacity);
	if (rd == -1)
		return (FLOW_ERROR);
	if (rd == 0)
		return (FLOW_DONE);
	_contentLength += rd;
	return (FLOW_MORE);
}
