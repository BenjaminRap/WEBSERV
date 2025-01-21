#ifndef FLOW_BUFFER_HPP
# define FLOW_BUFFER_HPP

# include <stddef.h>
# include <string>

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
	 * @brief The buffer used to store what as been read, or what will be sent.
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
	FlowBuffer(char *buffer, size_t bufferCapacity);
	~FlowBuffer();

	ssize_t	redirectContent(int srcFd, FdType srcType, int destFd, FdType destType);
	ssize_t	redirectContentFromBuffer(int destFd, FdType destType);
	// ssize_t	redirectContentToBuffer(int srcFd, FdType destType);
};

#endif // !FLOW_BUFFER_HPP