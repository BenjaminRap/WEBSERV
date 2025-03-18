#include <stddef.h>         // for NULL, size_t
#include <iostream>         // for operator<<, basic_ios, basic_ostream, cout

#include "ABody.hpp"        // for ABody
#include "FlowBuffer.hpp"   // for FlowState, FlowBuffer
#include "RawResponse.hpp"  // for RawResponse

/*************************Constructors / Destructors***************************/

/**
 * @brief Create a RawResponse instance. This class takes responsability
 * for closing the fd and deallocating the first part buffer.
 * @throw This function throw (std::logic_error) if bufferLength is superior to
 * bufferCapacity, if the buffer is null or if the bufferCapacity is set to 0.
 * In the case of a throw, the fd isn't closed and the firstPart isn't deallocated.
 * @param firstPart The first part of the response. It is composed by the status line,
 * the headers, the empty line and, maybe, a part of the body. It must be allocated
 * on the heap.
 * @param firstPartLength The length of firstPart.
 * @param bodyFd The fd of the body.
 * @param bodyFlowBuffer The FlowBuffer used to redirect the data from the body to
 * the client socket.
 */
RawResponse::RawResponse
(
	char *firstPart,
	size_t firstPartLength,
	ABody *body,
	FlowBuffer &bodyFlowBuffer
) :
	_firstPart(firstPart, firstPartLength, firstPartLength),
	_body(body),
	_bodyBuffer(&bodyFlowBuffer)
{

}

/**
 * @brief Create a RawResponse instance without body fd. This class takes responsability
 * for closing the fd and deallocating the first part buffer.
 * @throw This function throw (std::logic_error) if bufferLength is superior to
 * bufferCapacity, if the buffer is null or if the bufferCapacity is set to 0.
 * In the case of a throw, the fd isn't closed and the firstPart isn't deallocated.
 * @param firstPart The first part of the response. It is composed by the status line,
 * the headers, the empty line and, maybe, a part of the body. It must be allocated
 * on the heap.
 * @param firstPartLength The length of firstPart.
 */
RawResponse::RawResponse
(
	char *firstPart,
	size_t firstPartLength
) :
	_firstPart(firstPart, firstPartLength, firstPartLength),
	_body(NULL),
	_bodyBuffer(NULL)
{

}

/**
 * @brief The destructor of the RawResponse. This class takes responsability
 * for closing the fd and deallocating the firstPart buffer.
 */
RawResponse::~RawResponse()
{
	if (_body != NULL)
	{
		delete _body;
	}
	delete [] _firstPart.getBuffer();
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
	if (_firstPart.getBufferLength() != 0)
	{
		const FlowState flowState = _firstPart.redirectBufferContentToFd(socketFd);

		if (flowState == FLOW_DONE)
			return ((_body == NULL) ? FLOW_DONE : FLOW_MORE);
		return (flowState);
	}
	if (_body == NULL)
		return (FLOW_DONE);
	if (_body->getIsBlocking() == false)
		std::cout << "bouh !";
		//reading from the body
	//writing to the socket
	return (FLOW_DONE);
}
