#include "RawResponse.hpp"

/*************************Constructors / Destructors***************************/

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

/**
 * @brief Create a RawResponse instance without body fd.
 * @param buffer The buffer used to store the firstPart.
 * @param bufferCapacity The maximum number of chars that the buffer can store.
 * @param bodyBuffer The FlowBuffer used to redirect the data from the body to
 * the client socket.
 */
RawResponse::RawResponse
(
	char *buffer,
	size_t bufferCapacity,
	FlowBuffer &bodyBuffer
) :
	_firstPartBuffer(buffer, bufferCapacity),
	_bodyFd(-1),
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

/*******************************Member functions*******************************/

/**
 * @brief Send this response to the client socket.
 * @param socketFd The socket of the client, in which we will send the response.
 * @return -1 on error, 0 if the response has been entirely written and >0 if
 * there is more to write. In the latter case, we need to wait for another EPOLLOUT
 * before calling this fuction again, until we receive <=0.
 */
ssize_t	RawResponse::sendResponseToSocket(int socketFd)
{
	const ssize_t res = _firstPartBuffer.redirectContentFromBuffer(socketFd, SOCKETFD);

	if (res != 0)
		return (res);
	if (_bodyFd == -1)
		return (0);
	return (_bodyBuffer.redirectContent(_bodyFd, FILEFD, socketFd, SOCKETFD));
}