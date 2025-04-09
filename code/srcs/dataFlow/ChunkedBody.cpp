#include <algorithm>				// for std::find, std::min, std::distance
#include <unistd.h>					// for write

#include "ChunkedBody.hpp"
#include "requestStatusCode.hpp"	// for HTTP_...

/**********************Constructors/Destructors********************************/

ChunkedBody::ChunkedBody(int fd, Response &response, Request &request) :
	ABody(fd),
	_chunkSize(0),
	_response(response),
	_request(request)
{
}

ChunkedBody::~ChunkedBody()
{
}

/*************************Private Member Functions*****************************/

int		ChunkedBody::parseChunkSize(char *begin, char *end)
{
	(void)begin;
	(void)end;
	return (0);
}

ssize_t	ChunkedBody::readChunkedBodyLength(char *begin, char *end)
{
	char * const	lineBreak = std::find(begin, end, '\n');
	
	if (lineBreak == end)
		return (0);
	const int code = parseChunkSize(begin, lineBreak);
	if (code != HTTP_OK)
	{
		_response.setResponse(code);
		setFinished();
		_state = CHUNKED_DONE;
		return (-1);
	}
	if (_chunkSize == 0)
		_state = CHUNKED_TRAILERS;
	else
		_state = CHUNKED_CONTENT;
	return (std::distance(begin, lineBreak + 1));
}

ssize_t	ChunkedBody::writeChunkedBodyData(int fd, char *begin, char *end)
{
	const size_t	contentLength = std::distance(begin, end);
	const size_t	charsToWrite = std::min(contentLength, _chunkSize);
	const ssize_t	written = write(fd, begin, charsToWrite);

	if (written == -1)
	{
		_response.setResponse(HTTP_INTERNAL_SERVER_ERROR);
		setFinished();
		_state = CHUNKED_DONE;
		return (-1);
	}
	_chunkSize -= written;
	if (_chunkSize == 0)
		_state = CHUNKED_CONTENT_ENDLINE;
	return (written);
}


ssize_t	ChunkedBody::readChunkedBodyEndLine(char *begin, char *end)
{
	if ((size_t)std::distance(begin, end) < endLine.size())
		return (0);
	if (endLine.compare(0, endLine.size(), begin, endLine.size()) != 0)
	{
		_response.setResponseCode(400, "Bad Request");
		setFinished();
		_state = CHUNKED_DONE;
		return (-1);
	}
	_state = CHUNKED_SIZE;
	return (endLine.size());
}


ssize_t	ChunkedBody::readTrailer(char *begin, char *end)
{
	char * const	pos = std::find(begin, end, '\n');
	
	if (pos == end)
		return (0);
	if (pos == begin)
	{
		setFinished();
		_state = CHUNKED_DONE;
		return (endLine.size());
	}
	const int	ret = _request.parseHeader(begin, pos);
	if (ret != 0)
	{
		if (ret  == -1)
			_response.setResponse(HTTP_BAD_REQUEST);
		else
			_response.setResponse(HTTP_INTERNAL_SERVER_ERROR);
		setFinished();
		_state = CHUNKED_DONE;
		return (-1);
	}
	return (std::distance(begin, pos + endLine.size()));
}

ssize_t	ChunkedBody::writeToFd(const void* buffer, size_t bufferCapacity)
{
	ssize_t	totalWritten = 0;
	char * end = (char*)buffer + bufferCapacity;
	
	while (_state != CHUNKED_TRAILERS && _state != CHUNKED_DONE)
	{
		if (_state == CHUNKED_SIZE)
		{
			const ssize_t	written = readChunkedBodyLength((char*)buffer + totalWritten, end);
			if (written == 0)
				return (totalWritten);
			else if (written == -1)
				return (-1);
			totalWritten += written;
		}
		if (_state == CHUNKED_CONTENT)
		{
			const ssize_t	written = writeChunkedBodyData(getFd(), (char*)buffer + totalWritten, end);
			if (written == 0)
				return (totalWritten);
			else if (written == -1)
				return (-1);
			totalWritten += written;
		}
		if (_state == CHUNKED_CONTENT_ENDLINE)
		{
			const ssize_t	written = readChunkedBodyEndLine((char*)buffer + totalWritten, end);
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
