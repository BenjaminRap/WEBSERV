#include "ChunkedBody.hpp"


ChunkedBody::ChunkedBody(int fd) :
	Body(fd)
{
}

ChunkedBody::~ChunkedBody()
{
}


FlowState	writeBodyFromBufferToFile(FlowBuffer &flowBuffer)
{

}

FlowState	redirectBodyFromSocketToFile(FlowBuffer &flowBuffer, int socketFd)
{

}