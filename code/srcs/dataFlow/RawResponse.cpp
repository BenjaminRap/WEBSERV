#include "RawResponse.hpp"

/*************************Constructors / Destructors***************************/

/**
 * @brief Create a RawResponse instance.
 * @throw This function throw (std::logic_error) if the buffer is null or if
 * the bufferCapacity is set to 0.
 * @param buffer The buffer used to store the firstPart.
 * @param bufferCapacity The maximum number of chars that the buffer can store.
 * @param bodyFd The fd of the body.
 * @param bodyBuffer The FlowBuffer used to redirect the data from the body to
 * the client socket.
 */
RawResponse::RawResponse
(
	char *firstPart,
	size_t firstPartLength,
	int bodyFd,
	FlowBuffer &bodyBuffer
) :
	_firstPartBuffer(firstPart, firstPartLength, firstPartLength),
	_bodyFd(bodyFd),
	_bodyBuffer(bodyBuffer)
{

}

/**
 * @brief Create a RawResponse instance without body fd.
 *  * @throw This function throw (std::logic_error) if the buffer is null or if
 * the bufferCapacity is set to 0.
 * @param buffer The buffer used to store the firstPart.
 * @param bufferCapacity The maximum number of chars that the buffer can store.
 * @param bodyBuffer The FlowBuffer used to redirect the data from the body to
 * the client socket.
 */
RawResponse::RawResponse
(
	char *firstPart,
	size_t firstPartLength,
	FlowBuffer &bodyBuffer
) :
	_firstPartBuffer(firstPart, firstPartLength, firstPartLength),
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
 * @return FLOW_ERROR on error, FLOW_DONE if the response has been entirely written
 * and FLOW_EAGAIN_SEND if there is more to send . In the latter case, we need
 * to wait for another EPOLLOUT before calling this fuction again, until we
 * receive FLOW_ERROR or FLOW_DONE.
 */
FlowState	RawResponse::sendResponseToSocket(int socketFd)
{
	FdType			destType = SOCKETFD;
	FdType			srcType = FILEFD;
	const FlowState flowState = _firstPartBuffer.redirectContentFromBuffer(socketFd, destType);

	if (flowState != FLOW_DONE)
		return (flowState);
	if (_bodyFd == -1)
		return (FLOW_DONE);
	return (_bodyBuffer.redirectContent(_bodyFd, srcType, socketFd, destType));
}