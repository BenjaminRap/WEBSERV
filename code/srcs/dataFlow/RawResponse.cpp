#include <stddef.h>         // for NULL, size_t

#include "ABody.hpp"        // for ABody
#include "FlowBuffer.hpp"   // for FlowState, FlowBuffer
#include "Response.hpp"		// for Response
#include "protocol.hpp"		// for PROTOCOL, PROTOCOL_LENGTH
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
	ABody &body,
	FlowBuffer &bodyFlowBuffer,
	bool isBlocking,
	int srcBodyFd
) :
	_firstPartLength(firstPartLength),
	_firstPart(firstPart, firstPartLength, firstPartLength),
	_isBlocking(isBlocking),
	_srcBodyFd(srcBodyFd),
	_body(&body),
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
	_firstPartLength(firstPartLength),
	_firstPart(firstPart, firstPartLength, firstPartLength),
	_body(NULL),
	_bodyBuffer(NULL)
{

}

RawResponse::RawResponse(Response &response) :
	_firstPartLength(getFirstPartLength(response)),
	_firstPart((char*)5, 5, 5)
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


size_t	RawResponse::getFirstPartLength(const Response &response)
{
	size_t										length = 0;
	const std::map<std::string, std::string>	headers = response.getHeaderMap();

	length += PROTOCOL_LENGTH;
	length += 1; // + 1 for the space
	length += 3; // 3 for the code length
	length += 1; // + 1 for the space
	length += response.getStatusText().size();
	length += LINE_END_LENGTH;
	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++) {
		length += it->first.size() + it->second.size();
		length += LINE_END_LENGTH;
	}
	length += LINE_END_LENGTH; // for the empty line
	length += 1; // for the /0
	return (length);
}

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
	if (_isBlocking == false)
		return (_bodyBuffer->redirectContent<int, ABody&>(_srcBodyFd, *_body, ABody::callInstanceWriteToFd));
	return (_bodyBuffer->redirectBufferContentToFd<ABody&>(*_body, ABody::callInstanceWriteToFd));
}
