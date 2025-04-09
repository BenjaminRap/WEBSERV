#ifndef CHUNKED_BODY_HPP
# define CHUNKED_BODY_HPP

# include "ABody.hpp"
# include "Response.hpp"
# include "Request.hpp"

enum ChunkedBodyState
{
	CHUNKED_SIZE,
	CHUNKED_CONTENT,
	CHUNKED_CONTENT_ENDLINE,
	CHUNKED_TRAILERS,
	CHUNKED_DONE
};

class ChunkedBody : public ABody
{
private:
	size_t				_chunkSize;
	ChunkedBodyState	_state;
	Response			&_response;
	Request				&_request;

	ChunkedBody();
	ChunkedBody(const ChunkedBody &chunkedBody);
	

	ssize_t	readChunkedBodyLength(char *start, char *last);
	ssize_t	writeChunkedBodyData(int fd, char *start, char *last);
	ssize_t	readChunkedBodyEndLine(char *start, char *last);
	ssize_t	readTrailer(char *start, char *last);
	int		parseChunkSize(char *start, char *end);
public:
	ChunkedBody(int fd, Response &response, Request &request);
	~ChunkedBody();
	
	ssize_t		writeToFd(const void* buffer, size_t bufferCapacity);
};

#endif // !CHUNKED_BODY_HPP
