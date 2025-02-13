#include <algorithm>

#include "ChunkedBody.hpp"

const std::string	ChunkedBody::endLine("\n\r");

/**********************Constructors/Destructors********************************/

ChunkedBody::ChunkedBody(int fd, Response &response) :
	Body(fd),
	_chunkSize(0),
	_response(response)
{
}

ChunkedBody::~ChunkedBody()
{
}

/*************************Static functions*************************************/

ssize_t	ChunkedBody::writeToFile(int fd, char *buffer, size_t count, ChunkedBody &chunkedBody)
{
	return (chunkedBody.writeToFd(fd, buffer, buffer + count));
}

/*************************Private Member Functions*****************************/

int		ChunkedBody::parseChunkSize(char *start, char *end)
{
	(void)start;
	(void)end;
	return (0);
}

ssize_t	ChunkedBody::readChunkedBodyLength(char *start, char *last)
{
	if (_state != CHUNKED_SIZE)
		return (0);
	char * const	pos = std::search(start, last, endLine.begin(), endLine.end());
	
	if (pos == last)
		return (0);
	const int ret = parseChunkSize(start, pos);
	if (ret != 0)
	{
		if (ret  == -1)
			_response.setResponseCode(400, "Bad Request");
		else
			_response.setResponseCode(500, "Internal Server Error");
		return (-1);
	}
	if (_chunkSize == 0)
		_state = CHUNKED_TRAILERS;
	else
		_state = CHUNKED_CONTENT;
	return (std::distance(start, pos + endLine.size()));
}

ssize_t	ChunkedBody::writeChunkedBodyData(int fd, char *start, char *last)
{
	if (_state != CHUNKED_CONTENT)
		return (0);
	const size_t	bufferLength = std::distance(start, last);
	const size_t	charsToWrite = std::min(bufferLength, _chunkSize);
	const ssize_t	written = write(fd, start, charsToWrite);
	if (written == -1)
		return (-1);
	_chunkSize -= written;
	if (_chunkSize == 0)
		_state = CHUNKED_CONTENT_ENDLINE;
	return (written);
}


ssize_t	ChunkedBody::readChunkedBodyEndLine(char *start, char *last)
{
	if (_state != CHUNKED_CONTENT_ENDLINE
		|| (size_t)std::distance(start, last) < endLine.size())
	{
		return (0);
	}
	if (endLine.compare(0, endLine.size(), start, endLine.size()) != 0)
	{
		_response.setResponseCode(400, "Bad Request");
		return (-1);
	}
	_state = CHUNKED_SIZE;
	return (endLine.size());
}

ssize_t	ChunkedBody::writeToFd(int fd, char *start, char *end)
{
	static const std::string	endLine("\n\r");
	ssize_t						totalWritten = 0;
	
	{
		const ssize_t	written = readChunkedBodyLength(start, end);
		if (written == -1)
			return (-1);
		totalWritten += written;
	}
	{
		const ssize_t	written = writeChunkedBodyData(fd, start + totalWritten, end);
		if (written == -1)
			return (-1);
		totalWritten += written;
	}
	{
		const ssize_t	written = readChunkedBodyEndLine(start + totalWritten, end);
		if (written == -1)
			return (-1);
		totalWritten += written;
	}
	return (0);
}

/*************************Public Member Functions******************************/

FlowState	ChunkedBody::writeBodyFromBufferToFile(FlowBuffer &flowBuffer)
{

	return (flowBuffer.redirectBufferContentToFd(getFd(), *this, ChunkedBody::writeToFile));
}

FlowState	ChunkedBody::redirectBodyFromSocketToFile(FlowBuffer &flowBuffer, int socketFd)
{
	FdType	socketType = SOCKETFD;

	return (flowBuffer.redirectContent(socketFd, socketType, getFd(), *this, \
		readFromFdWithType,
		ChunkedBody::writeToFile));
}