#include <sys/socket.h>
#include <stdexcept>
#include <unistd.h>
#include <cerrno>

#include "RawResponse.hpp"

/**
 * @brief Create a RawResponse instance.
 * @param buffer The buffer used to store the firstPart.
 * @param bufferCapacity The maximum number of chars that the buffer can store.
 * @param bodyFd The fd of the body.
 * @param bodyBuffer The FlowBuffer used to redirect the data from the body to
 * the client socket.
 */
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