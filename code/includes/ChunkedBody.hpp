#ifndef CHUNKED_BODY_HPP
# define CHUNKED_BODY_HPP

# include "ABody.hpp"
# include "Request.hpp"

enum ChunkedBodyState
{
	CHUNKED_SIZE,
	CHUNKED_CONTENT,
	CHUNKED_ENDLINE,
	CHUNKED_TRAILERS,
	CHUNKED_DONE
};

class ChunkedBody : public ABody
{
private:
	static const std::string	_lineEnd;

	ssize_t						_chunkSize;
	ChunkedBodyState			_state;
	Request&					_request;

	ChunkedBody();
	ChunkedBody(const ChunkedBody &chunkedBody);
	

	ssize_t	readLength(char *begin, char *end);
	ssize_t	writeData(char *begin, char *end);
	ssize_t	readEndLine(char *begin, char *end);
	ssize_t	readTrailer(char *begin, char *end);

	void	setFinished(uint16_t status);
public:
	ChunkedBody(int fd, Request &request);
	~ChunkedBody();
	
	ssize_t		writeToFd(const void* buffer, size_t bufferCapacity);
};

#endif // !CHUNKED_BODY_HPP
