#ifndef CHUNKED_BODY_HPP
# define CHUNKED_BODY_HPP

# include "Body.hpp"

enum ChunkedBodyState
{
	CHUNKED_BODY_LENGTH,
	CHUNKED_BODY_CONTENT,
	CHUNKED_BODY_EMPTY,
	CHUNKED_BODY_TRAILERS
};

class ChunkedBody : public Body
{
private:
	ChunkedBody();
	ChunkedBody(const ChunkedBody &chunkedBody);
public:
	ChunkedBody(int fd);
	~ChunkedBody();

	FlowState	writeBodyFromBufferToFile(FlowBuffer &flowBuffer);
	FlowState	redirectBodyFromSocketToFile(FlowBuffer &flowBuffer, int socketFd);
};

#endif // !CHUNKED_BODY_HPP