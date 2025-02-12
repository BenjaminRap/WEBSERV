#ifndef CHUNKED_BODY_HPP
# define CHUNKED_BODY_HPP

# include "Body.hpp"
# include "Response.hpp"

enum ChunkedBodyState
{
	CHUNKED_SIZE,
	CHUNKED_CONTENT,
	CHUNKED_EMPTY,
	CHUNKED_TRAILERS,
	CHUNKED_DONE
};

class ChunkedBody : public Body
{
private:
	size_t				_chunkSize;
	ChunkedBodyState	_state;
	Response			&_response;

	ChunkedBody();
	ChunkedBody(const ChunkedBody &chunkedBody);
	

	ssize_t	readChunkedBodyLength(char *buffer, size_t bufferCapacity);
	ssize_t	writeChunkedBodyData(int fd, char *buffer, size_t bufferCapacity);
	int		parseChunkSize(char *start, char *end);
public:
	ChunkedBody(int fd, Response &response);
	~ChunkedBody();
	
	static ssize_t	writeToFile(int fd, char *buffer, size_t bufferCapacity, ChunkedBody &chunkedBody);
	
	ssize_t		writeToFd(int fd, char *buffer, size_t bufferCapacity);
	FlowState	writeBodyFromBufferToFile(FlowBuffer &flowBuffer);
	FlowState	redirectBodyFromSocketToFile(FlowBuffer &flowBuffer, int socketFd);
};

#endif // !CHUNKED_BODY_HPP