#include <stddef.h>        // for size_t, NULL
#include <sys/socket.h>    // for MSG_DONTWAIT, MSG_NOSIGNAL, recv, send
#include <sys/types.h>     // for ssize_t
#include <unistd.h>        // for read, write
#include <stdexcept>       // for logic_error
#include <string>          // for basic_string

#include "FlowBuffer.hpp"  // for FlowBuffer, FdType, readFromFdWithType

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

size_t		FlowBuffer::getBufferLength(void) const
{
	return (_bufferLength);
}


size_t		FlowBuffer::getNumCharsWritten(void) const
{
	return (_numCharsWritten);
}

const char	*FlowBuffer::getBuffer() const
{
	return (_buffer);
}

/**
 * @brief  Get a line, (finishing by \n). Return true if there was a line in the
 * buffer, false otherwise. If there is a line, lineStart is a pointer on the
 * start of this line, and length is the lenth of the line. If there is no line,
 * lineStart is set to NULL and length to -1.
 * This functions update the FlowBuffer internal _numCharsWritten variables, 
 * meaning that the line that has been returned, is counted as read and won't be
 * returned again.
 */
bool		FlowBuffer::getLine(char **lineStart, ssize_t *length)
{
	size_t	index;

	index = _numCharsWritten;
	while (index < _bufferLength)
	{
		if (_buffer[index] == '\n')
		{
			*lineStart = _buffer + _numCharsWritten;
			*length = index - _numCharsWritten;
			_numCharsWritten = index + 1;
			if (_numCharsWritten == _bufferLength)
			{
				_numCharsWritten = 0;
				_bufferLength = 0;
			}
			return (true);
		}
		index++;
	}
	*lineStart = NULL;
	*length = -1;
	return (false);
}

/************************FlowBuffer write/read functions***********************/

/**
 * @brief Read the data from fd.
 * @param fd The file descriptor to read from.
 * @param buffer The buffer in which the read bytes will be stored.
 * @param bufferCapacity The capacity of buffer.
 * @param fdType The type of fd, either FILEFD or SOCKETFD. It is used to determine
 * if the function will use recv (for sockets) or read (for files).
 * @return Return the number of bytes read, or -1 on error.
 */
ssize_t	readFromFdWithType(int fd, char *buffer, size_t bufferCapacity, FdType &fdType)
{
	if (fdType == SOCKETFD)
		return (recv(fd, buffer, bufferCapacity, MSG_DONTWAIT | MSG_NOSIGNAL));
	return (read(fd, buffer, bufferCapacity));
}

/**
 * @brief Write the data to fd.
 * @param fd The file descriptor to write into.
 * @param buffer The buffer containing all the bytes to write.
 * @param bufferCapacity The capacity of buffer.
 * @param fdType The type of fd, either FILEFD or SOCKETFD. It is used to determine
 * if the function will use send (for sockets) or write (for files).
 * @return Return the number of bytes read, or -1 on error.
 * @note When writing to a file, write() can return a value in the range [0, bufferCapacity].
 * The causes can be multiples (see man 2 write).It this case, we return an error.
 */
ssize_t	writeToFdWithType(int fd, char *buffer, size_t bufferCapacity, FdType &fdType)
{
	if (fdType == SOCKETFD)
		return (send(fd, buffer, bufferCapacity, MSG_DONTWAIT | MSG_NOSIGNAL));
	const ssize_t written = write(fd, buffer, bufferCapacity);
	return (written == bufferCapacity ? written : -1);
}