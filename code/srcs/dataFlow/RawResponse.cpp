#include "RawResponse.hpp"

/*************************Constructors / Destructors***************************/

/**
 * @brief Create a RawResponse instance.
 * @throw This function throw (std::logic_error) if the buffer is null or if
 * the bufferCapacity is set to 0.
 * @param firstPart The first part of the response. It is composed by the status line,
 * the headers, the empty line and, maybe, a part of the body.
 * @param firstPartLength The length of firstPart.
 * @param bodyFd The fd of the body.
 * @param bodyFlowBuffer The FlowBuffer used to redirect the data from the body to
 * the client socket.
 */
RawResponse::RawResponse
(
	char *firstPart,
	size_t firstPartLength,
	int bodyFd,
	FlowBuffer &bodyFlowBuffer
) :
	_firstPart(firstPart, firstPartLength, firstPartLength),
	_bodyFd(bodyFd),
	_bodyBuffer(&bodyFlowBuffer)
{

}

/**
 * @brief Create a RawResponse instance without body fd.
 *  * @throw This function throw (std::logic_error) if the buffer is null or if
 * the bufferCapacity is set to 0.
 * @param firstPart The first part of the response. It is composed by the status line,
 * the headers, the empty line and, maybe, a part of the body.
 * @param firstPartLength The length of firstPart.
 */
RawResponse::RawResponse
(
	char *firstPart,
	size_t firstPartLength
) :
	_firstPart(firstPart, firstPartLength, firstPartLength),
	_bodyFd(-1),
	_bodyBuffer(NULL)
{

}

RawResponse::RawResponse(const RawResponse& ref) :
	_firstPart(ref._firstPart),
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
 * and FLOW_MORE if there is more to send . In the latter case, we need
 * to wait for another EPOLLOUT before calling this fuction again, until we
 * receive FLOW_ERROR or FLOW_DONE.
 */
FlowState	RawResponse::sendResponseToSocket(int socketFd)
{
	FdType			destType = SOCKETFD;
	FdType			srcType = FILEFD;
	const FlowState flowState = _firstPart.redirectBufferContentToFd(socketFd, destType);

	if (flowState != FLOW_DONE)
		return (flowState);
	if (_bodyFd == -1)
		return (FLOW_DONE);
	return (_bodyBuffer->redirectContent(_bodyFd, srcType, socketFd, destType));
}