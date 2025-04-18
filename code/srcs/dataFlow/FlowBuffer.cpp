#include <cstring>			// for std::memmove
#include <stddef.h>        // for size_t, NULL
#include <algorithm>       // for find
#include <iterator>        // for distance
#include <stdexcept>       // for logic_error
#include <string>          // for basic_string

#include "FlowBuffer.hpp"  // for FlowBuffer

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

FlowBuffer::~FlowBuffer()
{

}

/*****************************Member Functions*********************************/

bool		FlowBuffer::getLine(char **lineStart, size_t *length)
{
	char * const	start = _buffer + _numCharsWritten;
	char * const	afterEnd = _buffer + _bufferLength;
	char * const	breakline = std::find(start, afterEnd, '\n');

	if (breakline == afterEnd)
		return (false);
	*lineStart = start;
	*length = std::distance(start, breakline);
	_numCharsWritten += std::distance(start, breakline + 1); // +1 because we go past the /n
	if (breakline == afterEnd - 1) // afterEnd - 1 means the last character
	{
		_numCharsWritten = 0;
		_bufferLength = 0;
	}
	return (true);
}

void	FlowBuffer::moveBufferContentToStart(void)
{
	if (_numCharsWritten == 0)
		return ;
	std::memmove(_buffer, _buffer + _numCharsWritten, _bufferLength);
	_numCharsWritten = 0;
}

/**********************************Getters******************************************/

size_t		FlowBuffer::getBufferLength(void) const
{
	return (_bufferLength);
}

size_t		FlowBuffer::getBufferCapacity(void) const
{
	return (_bufferCapacity);
}

size_t		FlowBuffer::getNumCharsWritten(void) const
{
	return (_numCharsWritten);
}

const char	*FlowBuffer::getBuffer() const
{
	return (_buffer);
}
