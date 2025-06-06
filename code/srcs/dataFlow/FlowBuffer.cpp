#include <algorithm>       // for find, min
#include <cstring>         // for size_t, memcpy, memmove, NULL
#include <iterator>        // for distance
#include <stdexcept>       // for logic_error
#include <string>          // for basic_string

#include "FlowBuffer.hpp"  // for FlowBuffer, MAX_CHARS_WRITTEN

/************************Constructors / Destructors****************************/

FlowBuffer::FlowBuffer(char *buffer, size_t bufferCapacity, size_t contentLength) :
	_buffer(buffer),
	_bufferCapacity(bufferCapacity),
	_contentLength(contentLength),
	_numCharsWritten(0)
{
	if (contentLength > bufferCapacity)
		throw std::logic_error("FlowBuffer constructor called with a contentLength superior to the bufferCapacity");
	if (_buffer == NULL && _bufferCapacity != 0)
		throw std::logic_error("FlowBuffer constructor called with a NULL buffer and a postive bufferCapacity");
}

FlowBuffer::~FlowBuffer()
{

}

/*****************************Member Functions*********************************/

bool		FlowBuffer::getLine(char **lineBegin, char **lineEnd)
{
	char * const	start = _buffer + _numCharsWritten;
	char * const	afterEnd = _buffer + _contentLength;
	char * const	breakline = std::find(start, afterEnd, '\n');

	if (breakline == afterEnd)
		return (false);
	*lineBegin = start;
	*lineEnd = breakline;
	_numCharsWritten += std::distance(start, breakline + 1); // +1 because we go past the /n
	if (_numCharsWritten >= _contentLength) // afterEnd - 1 means the last character
	{
		_numCharsWritten = 0;
		_contentLength = 0;
	}
	return (true);
}

void	FlowBuffer::getContent(char** lineBegin, char** lineEnd)
{
	*lineBegin = _buffer + _numCharsWritten;
	*lineEnd = _buffer + _contentLength;
	_numCharsWritten = 0;
	_contentLength = 0;
}

void	FlowBuffer::moveBufferContentToStart(void)
{
	if (_numCharsWritten == 0)
		return ;
	std::memmove(_buffer, _buffer + _numCharsWritten, _contentLength - _numCharsWritten);
	_contentLength -= _numCharsWritten;
	_numCharsWritten = 0;
}


void	FlowBuffer::setBuffer(char* buffer, size_t contentLength, size_t bufferCapacity)
{

	if (contentLength > bufferCapacity)
		throw std::logic_error("FlowBuffer setBuffer called with a contentLength superior to the bufferCapacity");
	if (_buffer == NULL && _bufferCapacity != 0)
		throw std::logic_error("FlowBuffer constructor called with a NULL buffer and a postive bufferCapacity");
	_buffer = buffer;
	_contentLength = contentLength;
	_bufferCapacity = bufferCapacity;
	_numCharsWritten = 0;
}

size_t	FlowBuffer::addContent(char* buffer, size_t bufferSize)
{

	const size_t	remainingCapacity = _bufferCapacity - _contentLength;
	if (bufferSize > remainingCapacity
		&&_numCharsWritten > MAX_CHARS_WRITTEN * _bufferCapacity)
	{
		moveBufferContentToStart();
	}
	const size_t	numCharsToCopy = std::min(remainingCapacity, bufferSize);

	if (numCharsToCopy == 0)
		return (0);
	std::memcpy(_buffer + _contentLength, buffer, numCharsToCopy);

	_contentLength += numCharsToCopy;
	return (numCharsToCopy);
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


bool	FlowBuffer::isBufferEmpty() const
{
	return (_numCharsWritten == _contentLength);
}
