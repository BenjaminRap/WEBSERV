#include <cstring>			// for std::memmove
#include <stddef.h>        // for size_t, NULL
#include <algorithm>       // for find
#include <iterator>        // for distance
#include <stdexcept>       // for logic_error
#include <string>          // for basic_string

#include "FlowBuffer.hpp"  // for FlowBuffer

/************************Constructors / Destructors****************************/

FlowBuffer::FlowBuffer(char *buffer, size_t bufferCapacity, size_t contentLength) :
	_buffer(buffer),
	_bufferCapacity(bufferCapacity),
	_contentLength(contentLength),
	_numCharsWritten(0)
{
	if (contentLength > bufferCapacity)
		throw std::logic_error("FlowBuffer constructor called with a contentLength superior to the bufferCapacity");
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
	char * const	afterEnd = _buffer + _contentLength;
	char * const	breakline = std::find(start, afterEnd, '\n');

	if (breakline == afterEnd)
		return (false);
	*lineStart = start;
	*length = std::distance(start, breakline);
	_numCharsWritten += std::distance(start, breakline + 1); // +1 because we go past the /n
	if (breakline == afterEnd - 1) // afterEnd - 1 means the last character
	{
		_numCharsWritten = 0;
		_contentLength = 0;
	}
	return (true);
}

void	FlowBuffer::moveBufferContentToStart(void)
{
	if (_numCharsWritten == 0)
		return ;
	std::memmove(_buffer, _buffer + _numCharsWritten, _contentLength - _numCharsWritten);
	_contentLength -= _numCharsWritten;
	_numCharsWritten = 0;
}

/**********************************Getters******************************************/

size_t		FlowBuffer::getContentLength(void) const
{
	return (_contentLength);
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

bool	FlowBuffer::isBufferFull() const
{
	return (_contentLength >= _bufferCapacity);
}
