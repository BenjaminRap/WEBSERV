#include <cstdio>			// for sprintf
#include <stddef.h>         // for NULL, size_t

#include "ABody.hpp"        // for ABody
#include "FlowBuffer.hpp"   // for FlowState, FlowBuffer
#include "Response.hpp"		// for Response
#include "protocol.hpp"		// for PROTOCOL, PROTOCOL_LENGTH
#include "RawResponse.hpp"  // for RawResponse

/*************************Constructors / Destructors***************************/

std::string	getFirstPart(const Response& response);

RawResponse::RawResponse(Response &response) :
	_firstPart(getFirstPart(response)),
	_firstPartBuffer(&_firstPart[0], _firstPart.size(), _firstPart.capacity()),
	_isBlocking(response.getIsBlocking()),
	_srcBodyFd(response.getSrcBodyFd()),
	_body(response.getBody())
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
	delete [] _firstPartBuffer.getBuffer();
}

/*******************************Member functions*******************************/

size_t	getFirstPartLength(const Response &response)
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

std::string	getFirstPart(const Response &response)
{
	const size_t								length = getFirstPartLength(response);
	const std::map<std::string, std::string>	headers = response.getHeaderMap();

	std::string									firstPart;
	char										statusCode[11];

	std::sprintf(statusCode, "%d", response.getStatusCode());
	firstPart.reserve(length);
	firstPart.append(PROTOCOL)
		.append(" ")
		.append(statusCode)
		.append(" ")
		.append(response.getStatusText())
		.append(LINE_END);

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); it++)
	{
		firstPart.append(it->first)
			.append(":")
			.append(it->second)
			.append(LINE_END);
	}
	firstPart.append(LINE_END);
	return (firstPart);
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
	if (_firstPartBuffer.getBufferLength() != 0)
	{
		const FlowState flowState = _firstPartBuffer.redirectBufferContentToFd(socketFd);

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
