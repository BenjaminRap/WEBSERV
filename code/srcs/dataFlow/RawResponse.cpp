#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <cerrno>

#include "RawResponse.hpp"

RawResponse::RawResponse
(
	char *buffer,
	size_t bufferCapacity,
	int bodyFd,
	FlowBuffer &bodyBuffer
) :
	_firstPartBuffer(buffer, bufferCapacity),
	_bodyFd(bodyFd),
	_bodyBuffer(bodyBuffer)
{

}

RawResponse::RawResponse(const RawResponse& ref) :
	_firstPartBuffer(ref._firstPartBuffer),
	_bodyFd(ref._bodyFd),
	_bodyBuffer(ref._bodyBuffer)
{

}

RawResponse::~RawResponse()
{

}