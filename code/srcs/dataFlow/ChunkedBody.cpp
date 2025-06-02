#include <stdint.h>                 // for uint16_t
#include <sys/types.h>              // for ssize_t
#include <algorithm>                // for search, min
#include <cctype>                   // for isxdigit
#include <cstring>                  // for size_t, memcmp
#include <iterator>                 // for distance
#include <string>                   // for basic_string, string

#include "ABody.hpp"                // for ABody, ABodyChilds
#include "ChunkedBody.hpp"          // for ChunkedBody, ChunkedBodyState
#include "requestStatusCode.hpp"    // for HTTP_BAD_REQUEST, HTTP_CONTENT_TO...
#include "socketCommunication.hpp"  // for checkError

unsigned long	strToULongBase(const char* begin, const char* end, int (&isInBase)(int character), int base);

const std::string	ChunkedBody::_lineEnd("\r\n");

/**********************Constructors/Destructors********************************/

ChunkedBody::ChunkedBody(int fd,  size_t maxSize, bool isFdBlocking) :
	ABody(fd, ABody::CHUNKED, isFdBlocking),
	_maxSize(maxSize),
	_chunkSize(-1),
	_state(ChunkedBody::SIZE)
{
}

ChunkedBody::ChunkedBody(size_t maxSize) :
	ABody(ABody::CHUNKED),
	_maxSize(maxSize),
	_chunkSize(-1),
	_state(ChunkedBody::SIZE)
{
}

ChunkedBody::~ChunkedBody()
{
}

/*************************Private Member Functions*****************************/


void	ChunkedBody::setFinished(uint16_t status)
{
	_state = ChunkedBody::DONE;
	ABody::setFinished(status);
}

static bool	doesAdditionOverflow(size_t a, size_t b)
{
	return (a > (size_t)-1 - b);
}

ssize_t	ChunkedBody::readSize(const char* begin, const char* end)
{
	const char*  const	lineBreak = std::search(begin, end, _lineEnd.begin(), _lineEnd.end());
	
	if (lineBreak == end)
		return (0);
	_chunkSize = strToULongBase(begin, lineBreak, std::isxdigit, 16);
	if (_chunkSize == (unsigned long)-1)
	{
		setFinished(HTTP_BAD_REQUEST);
		return (-1);
	}
	if (doesAdditionOverflow(getWritten(), _chunkSize)
		|| getWritten() + _chunkSize > _maxSize)
	{
		setFinished(HTTP_CONTENT_TOO_LARGE);
		return (-1);
	}
	if (_chunkSize == 0)
		_state = ChunkedBody::TRAILERS;
	else
		_state = ChunkedBody::DATA;
	return (std::distance(begin, lineBreak + _lineEnd.size()));
}

ssize_t	ChunkedBody::writeData(const char* begin, const char* end)
{
	const size_t	contentLength = std::distance(begin, end);
	const size_t	charsToWrite = std::min((size_t)_chunkSize, contentLength);
	const ssize_t	written = writeOrIgnore(begin, charsToWrite);

	if (checkError<ssize_t>(written, -1, "write() : "))
	{
		setFinished(HTTP_INTERNAL_SERVER_ERROR);
		return (-1);
	}
	_chunkSize -= written;
	if (_chunkSize == 0)
		_state = ChunkedBody::ENDLINE;
	return (written);
}


ssize_t	ChunkedBody::readEndLine(const char* begin, const char* end)
{
	if ((size_t)std::distance(begin, end) < _lineEnd.size())
		return (0);
	if (std::memcmp(begin, _lineEnd.c_str(), _lineEnd.size()))
	{
		setFinished(HTTP_BAD_REQUEST);
		return (-1);
	}
	_state = ChunkedBody::SIZE;
	return (_lineEnd.size());
}


ssize_t	ChunkedBody::readTrailer(const char* begin, const char* end)
{
	const char*  const	lineBreak = std::search(begin, end, _lineEnd.begin(), _lineEnd.end());
	
	if (lineBreak == end)
		return (0);
	if (lineBreak == begin)
	{
		setFinished(HTTP_OK);
		return (_lineEnd.size());
	}
	return (std::distance(begin, lineBreak + _lineEnd.size()));
}

typedef ssize_t (ChunkedBody::*ParserFunc)(const char* begin, const char* end);

ssize_t	ChunkedBody::writeChunkedRequestToFd(const char* begin, const char* end)
{
	static const ParserFunc			parsers[4] = {
		&ChunkedBody::readSize,
		&ChunkedBody::writeData,
		&ChunkedBody::readEndLine,
		&ChunkedBody::readTrailer
	};

	ssize_t	totalWritten = 0;

	while (_state != ChunkedBody::DONE)
	{
		const ParserFunc	parser = parsers[_state];
		const ssize_t 		written = (this->*parser)(begin + totalWritten, end);

		if (written == 0)
			return (totalWritten);
		else if (written == -1)
			return (-1);
		totalWritten += written;
	}
	return (totalWritten);
}

ssize_t	ChunkedBody::writeToFd(const void* buffer, size_t bufferCapacity)
{
	char*	begin = (char*)buffer;
	char*	end = begin + bufferCapacity;

	return (writeChunkedRequestToFd(begin, end));
}
