#ifndef FLOW_BUFFER_HPP
# define FLOW_BUFFER_HPP

# include <stddef.h>
# include <sys/socket.h>
# include <unistd.h>
# include <cerrno>

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
	 */
	size_t	_bufferLength;
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

	template <typename ReadData, typename WriteData>
	FlowState	redirectContent
	(
		ReadData readData,
		WriteData writeData,
		ssize_t (&customWrite)(WriteData writeData, const void *buffer, size_t bufferCapacity) = write,
		ssize_t (&customRead)(ReadData readData, void *buffer, size_t bufferCapacity) = read
	);
	template <typename WriteData>
	FlowState	redirectBufferContentToFd
	(
		WriteData writeData,
		ssize_t (&customWrite)(WriteData writeData, const void *buffer, size_t bufferCapacity) = write
	);
	template <typename ReadData>
	FlowState	redirectFdContentToBuffer
	(
		ReadData readData,
		ssize_t (&customRead)(ReadData readData, void *buffer, size_t bufferCapacity) = read
	);

	size_t		getBufferLength(void) const;
	size_t		getBufferCapacity(void) const;
	size_t		getNumCharsWritten(void) const;
	const char	*getBuffer() const;
	bool		getLine(char **lineStart, size_t *length);
};


# include "FlowBuffer.ipp"

#endif // !FLOW_BUFFER_HPP
