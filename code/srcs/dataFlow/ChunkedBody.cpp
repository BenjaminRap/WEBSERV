#include <algorithm>				// for std::find, std::min, std::distance
#include <cerrno>					// for errno
#include <unistd.h>					// for write
#include <cctype>					// for std::isxdigit

#include "ChunkedBody.hpp"
#include "requestStatusCode.hpp"	// for HTTP_...


long	getLongMax();
long	strToLongBase(const char *begin, const char* end, int (&isInBase)(int character), int base);

const std::string	ChunkedBody::_lineEnd("\r\n");

/**********************Constructors/Destructors********************************/

ChunkedBody::ChunkedBody(int fd,  Request &request) :
	ABody(fd),
	_chunkSize(-1),
	_request(request)
{
}

ChunkedBody::~ChunkedBody()
{
}

/*************************Private Member Functions*****************************/


void	ChunkedBody::setFinished(uint16_t status)
{
	_state = CHUNKED_DONE;
	ABody::setFinished(status);
}

ssize_t	ChunkedBody::readLength(char *begin, char *end)
{
	char * const	lineBreak = std::search(begin, end, _lineEnd.begin(), _lineEnd.end());
	
	if (lineBreak == end)
		return (0);
	const int chunkSize = strToLongBase(begin, lineBreak, std::isxdigit, 16);
	if (chunkSize == getLongMax())
	{
		setFinished(HTTP_BAD_REQUEST);
		return (-1);
	}
	_state = CHUNKED_CONTENT;
	return (std::distance(begin, lineBreak + _lineEnd.size()));
}

ssize_t	ChunkedBody::writeData(char *begin, char *end)
{
	const size_t	contentLength = std::distance(begin, end);
	const size_t	charsToWrite = std::min(contentLength, (size_t)_chunkSize);
	const ssize_t	written = write(getFd(), begin, charsToWrite);

	if (written == -1)
	{
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
		return (-1);
	}
	_chunkSize -= written;
	if (_chunkSize == 0)
		_state = CHUNKED_ENDLINE;
	return (written);
}


ssize_t	ChunkedBody::readEndLine(char *begin, char *end)
{
	if ((size_t)std::distance(begin, end) < _lineEnd.size())
		return (0);
	if (std::memcmp(begin, _lineEnd.c_str(), _lineEnd.size()))
	{
		setFinished(HTTP_BAD_REQUEST);
		return (-1);
	}
	if (_chunkSize == 0)
		_state = CHUNKED_TRAILERS;
	else
		_state = CHUNKED_SIZE;
	return (_lineEnd.size());
}


ssize_t	ChunkedBody::readTrailer(char *begin, char *end)
{
	char * const	lineBreak = std::search(begin, end, _lineEnd.begin(), _lineEnd.end());
	
	if (lineBreak == end)
		return (0);
	if (lineBreak == begin)
	{
		setFinished(HTTP_OK);
		return (_lineEnd.size());
	}
	const int	code = _request.parseHeader(begin, lineBreak);
	if (code != HTTP_OK)
	{
		setFinished(code);
		return (-1);
	}
	return (std::distance(begin, lineBreak + _lineEnd.size()));
}

ssize_t	ChunkedBody::writeToFd(const void* buffer, size_t bufferCapacity)
{
	ssize_t	totalWritten = 0;
	char * end = (char*)buffer + bufferCapacity;
	
	while (_state != CHUNKED_TRAILERS && _state != CHUNKED_DONE)
	{
		if (_state == CHUNKED_SIZE)
		{
			const ssize_t	written = readLength((char*)buffer + totalWritten, end);
			if (written == 0)
				return (totalWritten);
			else if (written == -1)
				return (-1);
			totalWritten += written;
		}
		if (_state == CHUNKED_CONTENT)
		{
			const ssize_t	written = writeData((char*)buffer + totalWritten, end);
			if (written == 0)
				return (totalWritten);
			else if (written == -1)
				return (-1);
			totalWritten += written;
		}
		if (_state == CHUNKED_ENDLINE)
		{
			const ssize_t	written = readEndLine((char*)buffer + totalWritten, end);
			if (written == 0)
				return (totalWritten);
			else if (written == -1)
				return (-1);
			totalWritten += written;
		}
	}
	while (_state == CHUNKED_TRAILERS)
	{
		const ssize_t	written = readTrailer((char*)buffer + totalWritten, end);
		if (written == 0)
			return (totalWritten);
		else if (written == -1)
			return (-1);
		totalWritten += written;
	}
	return (totalWritten);
}
