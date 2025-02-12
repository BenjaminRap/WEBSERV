#include <algorithm>

#include "ChunkedBody.hpp"

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

ssize_t	ChunkedBody::writeToFile(int fd, char *buffer, size_t bufferCapacity, ChunkedBody &chunkedBody)
{
	return (chunkedBody.writeToFd(fd, buffer, bufferCapacity));
}

/*************************Private Member Functions*****************************/

int		ChunkedBody::parseChunkSize(char *start, char *end)
{
	(void)start;
	(void)end;
	return (0);
}

ssize_t	ChunkedBody::readChunkedBodyLength(char *buffer, size_t bufferCapacity)
{
	if (_state != CHUNKED_SIZE)
		return (0);
	char * const	last = buffer + bufferCapacity;
	char * const	endLine = std::find(buffer, last, '\n');
	
	if (endLine == last)
		return (0);
	const int ret = parseChunkSize(buffer, endLine);
	if (ret != 0)
	{
		if (ret  == -1)
			_response.setResponseCode(400, "Bad Request");
		else
			_response.setResponseCode(500, "Internal Server Error");
		return (-1);
	}
	return (std::distance(buffer, last));
}

ssize_t	ChunkedBody::writeChunkedBodyData(int fd, char *buffer, size_t bufferCapacity)
{
}

ssize_t	ChunkedBody::writeToFd(int fd, char *buffer, size_t bufferCapacity)
{
	ssize_t	totalWritten = 0;
	
	{
		const ssize_t	written = readChunkedBodyLength(buffer, bufferCapacity);
		if (written == -1)
			return (-1);
		totalWritten += written;
	}
	{
		const ssize_t	written = writeChunkedBodyData(fd, buffer, bufferCapacity);
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