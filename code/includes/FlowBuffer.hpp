#ifndef FLOW_BUFFER_HPP
# define FLOW_BUFFER_HPP

# include <stddef.h>
# include <sys/socket.h>
# include <unistd.h>
# include <string>
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
 * @brief The differents type of a file descriptor.
 */
enum FdType
{
	SOCKETFD,
	FILEFD
};

ssize_t		writeToFdWithType(int fd, char *buffer, size_t bufferCapacity, FdType &fdType);
ssize_t		readFromFdWithType(int fd, char *buffer, size_t bufferCapacity, FdType &fdType);

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
public:
	FlowBuffer(const FlowBuffer& ref);
	FlowBuffer(char *buffer, size_t bufferCapacity, size_t bufferLength);
	~FlowBuffer();

	template <typename ReadData, typename WriteData>
	FlowState	redirectContent
	(
		int srcFd,
		ReadData &readData,
		int destFd,
		WriteData &writeData,
		ssize_t (&customRead)(int fd, char *buffer, size_t bufferCapacity, ReadData &readData) = readFromFdWithType,
		ssize_t (&customWrite)(int fd, char *buffer, size_t bufferCapacity, WriteData &writeData) = writeToFdWithType
	);
	template <typename WriteData>
	FlowState	redirectBufferContentToFd
	(
		int destFd,
		WriteData &writeData,
		ssize_t (&customWrite)(int fd, char *buffer, size_t bufferCapacity, WriteData &writeData) = writeToFdWithType
	);
	template <typename ReadData>
	FlowState	redirectFdContentToBuffer
	(
		int srcFd,
		ReadData &readData,
		ssize_t (&customRead)(int fd, char *buffer, size_t bufferCapacity, ReadData &readData) = readFromFdWithType
	);

	size_t		getBufferLength(void) const;
	size_t		getNumCharsWritten(void) const;
	const char	*getBuffer() const;
	bool		getLine(char **lineStart, size_t *length);
	void		moveContentToStartOfBuffer();
};


# include "FlowBuffer.tpp"

#endif // !FLOW_BUFFER_HPP