
#ifndef FLOW_BUFFER_HPP
# define FLOW_BUFFER_HPP

# include <stddef.h>
# include <sys/socket.h>
# include <unistd.h>
# include <cerrno>

/**
 * @brief The proportion of the buffer that can be already written
 * before the redirectFdContentToBuffer move everything to the start.
 */
# define MAX_CHARS_WRITTEN 0.2

/**
 * @brief The meanings of the FlowBuffer functions member's returns.
 */
enum FlowState
{
	/**
	 * @brief An error append, errno is set or an error has been written in the
	 * standart error output.
	 */
	FLOW_ERROR = -1,
	/**
	 * @brief There is nothing left to write/send or read/recv. This can occur if
	 * we read from a file and it is finished, if we read from the buffer and it is
	 * empty, or if we read from a socket and the other end closed the connection.
	 */
	FLOW_DONE,
	/**
	 * @brief There is more data to redirect. In this case we should wait for
	 * anoter EPOLLIN/EPOLLOUT event.
	 */
	FLOW_MORE,
	/**
	 * @brief The internal buffer is full, it can only occurs when redirecting
	 * data in the buffer and not out. This means that we can call the function
	 * again after emptying the buffer, without waiting for an EPOLLIN.
	 */
	FLOW_BUFFER_FULL
};

/**
 * @brief This class has a buffer and informations about the buffer. It also
 * has member functions to redirect content, using his internal buffer.
 */
class FlowBuffer
{
private:
	/**
	 * @brief The buffer used to store what has been read, or what will be sent.
	 */
	char	*_buffer;
	/**
	 * @brief The max number of chars the buffer can store without segfault.
	 */
	size_t	_bufferCapacity;
	/**
	 * @brief The number of chars that has been written in this buffer, the chars
	 * with an index superior to this number are undefined and shouldn't be use.
	 * This is a position relative to the buffer beginning, not to _numCharsWritten.
	 * contentLength is the position of the last character written by a read. It isn't
	 * affected when writing data.
	 */
	size_t	_contentLength;
	/**
	 * @brief The number of chars from the buffer that has already be written in
	 * the destination.
	 */
	size_t	_numCharsWritten;

	FlowBuffer();

	FlowBuffer&	operator=(const FlowBuffer &flowBuffer);
	FlowBuffer(const FlowBuffer& ref);
public:
	FlowBuffer(char *buffer, size_t bufferCapacity, size_t bufferLength);
	~FlowBuffer();

	/**
	 * @brief redirect the content from the source readData, to the destination
	 * writeData. To write the content of the source into the destination , it calls the
	 * customRead that read the readValue into the flowBuffer's internal buffer, and then
	 * call the customWrite to write the internal's buffer content into the writeData.
	 * if the customRead function fails, it doesn't call the customWrite function.
	 * @ref redirectFdContentToBuffer
	 * @ref redirectBufferContentToFd
	 *
	 * @param readData The source of the data.
	 * @param writeData The destination of the data
	 * @param customWrite The function used to write the data, the default value is write
	 * from unistd.h
	 * @param customRead The function used to read the data, the default value is read
	 * from unistd.h
	 * @return FLOW_ERROR if the customRead or customWrite returns -1
	 * FLOW_DONE if the customRead returns 0 the buffer has been entirely written.
	 * FLOW_MORE if the customRead returns 0, or there is more to write, or if the buffer
	 * was full and the customWrite freed some spaces.
	 * FLOW_BUFFER_FULL if the buffer is full and the customWrite return 0.
	 */
	template <typename ReadData, typename WriteData>
	FlowState	redirect
	(
		ReadData readData,
		WriteData writeData,
		ssize_t (&customWrite)(WriteData writeData, const void *buffer, size_t bufferCapacity) = write,
		ssize_t (&customRead)(ReadData readData, void *buffer, size_t bufferCapacity) = read
	);
	/**
	 * @brief redirect the content from the flowBuffer's internal buffer, to the destination
	 * writeData. To write the content of the buffer, it calls the customWrite function with
	 * writeData, the internal buffer and the number of characters to write.
	 * This functions reads from the buffer and write into the writeData, returning the number of
	 * characters written, or -1.
	 * If this function doesn't behave this way, the behaviour is undefined.
	 *
	 * @param writeData The destination of the data
	 * @param customWrite The function used to write the data, the default value is write
	 * from unistd.h
	 * @return FLOW_ERROR if customWrite returns -1
	 * FLOW_DONE if the buffer has been entirely written
	 * FLOW_MORE if the buffer has remaining characters to write.
	 * FLOW_BUFFER_FULL if the buffer is full and the customWrite return 0. That means that
	 * customWrite can't write, and we can't read.
	 */
	template <typename WriteData>
	FlowState	buffToDest
	(
		WriteData writeData,
		ssize_t (&customWrite)(WriteData writeData, const void *buffer, size_t bufferCapacity) = write
	);
	/**
	 * @brief redirect the content from a source readData, to the flowBuffer's internal
	 * buffer. To read the content of the source, it calls the customRead function with
	 * readData, the internal buffer and the internal buffer remaining capacity.
	 * This functions reads from readData and write into the buffer, returning the number of
	 * characters read, or -1.
	 * If this function doesn't behave this way, the behaviour is undefined.
	 *
	 * @param readData The source of the data
	 * @param customRead The function used to read the data, the default value is read
	 * from unistd.h
	 * @return FLOW_BUFFER_FULL if the buffer is full so nothing can be read.
	 * FLOW_ERROR if the customRead function returns -1.
	 * FLOW_DONE if the customRead function returns 0.
	 * FLOW_MORE otherwise.
	 */
	template <typename ReadData>
	FlowState	srcToBuff
	(
		ReadData readData,
		ssize_t (&customRead)(ReadData readData, void *buffer, size_t bufferCapacity) = read
	);

	void		setBuffer(char* buffer, size_t contentLength, size_t bufferCapacity);

	size_t		getContentLength(void) const;
	size_t		getBufferCapacity(void) const;
	size_t		getNumCharsWritten(void) const;
	const char	*getBuffer() const;
	bool		isBufferFull() const;
	bool		isBufferEmpty() const;

	/**
	 * @brief Get a line from this bufferFlow internal buffer.
	 * @note The \n isn't taken into account in the length and there may not have a \0
	 * after the \n. The internal buffer isn't changed.
	 *
	 * @param lineStart If the buffer contains a line, set this variable to the start
	 * of the line, otherwise, this variable isn't changed.
	 * @param length If the buffer contains a line, set this variable to the length
	 * of the line, otherwise, this variable isn't changed.
	 * @return True if there is a line, false otherwise.,
	 */
	bool		getLine(char **lineStart, size_t *length);
	/**
	 * @brief Move the content of the FlowBuffer to the start of the buffer,
	 * setting the number of characters written to 0. It can be useful if we don't
	 * want the _numCharsWritten to slowly reach the end of the buffer.
	 * This functions is called in redirectFdContentToBuffer, if the number of characters
	 * written exceeds MAX_CHARS_WRITTEN * _bufferCapacity.
	 */
	void		moveBufferContentToStart(void);
};


# include "FlowBuffer.ipp"

#endif // !FLOW_BUFFER_HPP
