#ifndef CHUNKED_BODY_HPP
# define CHUNKED_BODY_HPP

# include "ABody.hpp"
# include "Request.hpp"

enum ChunkedBodyState
{
	CHUNKED_SIZE,
	CHUNKED_DATA,
	CHUNKED_ENDLINE,
	CHUNKED_TRAILERS,
	CHUNKED_DONE
};

class ChunkedBody : public ABody
{
private:
	static const std::string	_lineEnd;

	const	size_t				_maxSize;

	size_t						_totalSize;
	size_t						_numCharsWritten;
	ssize_t						_chunkSize;
	ChunkedBodyState			_state;
	Request&					_request;

	ChunkedBody();
	ChunkedBody(const ChunkedBody &chunkedBody);
	

	ssize_t	readSize(const char* begin, const char* end);
	ssize_t	writeData(const char* begin, const char* end);
	ssize_t	readEndLine(const char* begin, const char* end);
	ssize_t	readTrailer(const char* begin, const char* end);
	ssize_t	writeChunkedRequestToFd(const char* begin, const char* end);

	void	setFinished(uint16_t status);
public:
	ChunkedBody(int fd, Request &request, size_t maxSize);
	~ChunkedBody();
	
	ssize_t		writeToFd(const void* buffer, size_t bufferCapacity);
};

#endif // !CHUNKED_BODY_HPP
