#ifndef FLOW_BUFFER_HPP
# define FLOW_BUFFER_HPP

# include <stddef.h>
# include <string>

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
	 * @brief There is more to receive, but we need to wait another EPOLLIN
	 * event. This can only append if we read from a socket.
	 */
	FLOW_MORE_RECV,
	/**
	 * @brief There is more to send, but we need to wait another EPOLLOUT
	 * event. This can only append if we write in a socket.
	 */
	FLOW_MORE_SEND,
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

	FlowState	redirectContent(int srcFd, FdType srcType, int destFd, FdType destType);
	FlowState	redirectContentFromBuffer(int destFd, FdType destType);
	FlowState	redirectContentToBuffer(int srcFd, FdType srcType);
	size_t		getBufferLength(void) const;
	size_t		getNumCharsWritten(void) const;
};

#endif // !FLOW_BUFFER_HPP