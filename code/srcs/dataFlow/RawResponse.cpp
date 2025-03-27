#include <cstdio>			// for sprintf
#include <stddef.h>         // for NULL, size_t
#include <unistd.h>			// for close

#include "ABody.hpp"        // for ABody
#include "FlowBuffer.hpp"   // for FlowState, FlowBuffer
#include "Response.hpp"		// for Response
#include "protocol.hpp"		// for PROTOCOL, PROTOCOL_LENGTH
#include "RawResponse.hpp"  // for RawResponse
#include "Status.hpp"		// for Status

/*************************Constructors / Destructors***************************/

std::string	getFirstPart(const Response& response);

RawResponse::RawResponse(Response &response, FlowBuffer &bodyBuffer) :
	_firstPart(getFirstPart(response)),
	_firstPartBuffer(&_firstPart[0], _firstPart.capacity(), _firstPart.length()),
	_isBlocking(response.getIsBlocking()),
	_srcBodyFd(response.getSrcBodyFd()), _body(response.getBody()),
	_bodyBuffer(bodyBuffer)
{
	
}

/**
 * @brief The destructor of the RawResponse. This class takes responsability
 * for closing the fd and deallocating the firstPart buffer.
 */
RawResponse::~RawResponse()
{
}

/*******************************Member functions*******************************/

size_t	getFirstPartLength(const std::map<std::string, std::string>& headers, const Status& status)
{
	size_t										length = 0;

	length += PROTOCOL_LENGTH;
	length += 1; // + 1 for the space
	length += 3; // 3 for the code length
	length += 1; // + 1 for the space
	length += status.getText().size();
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
	const Status * const						status = response.getStatus();

	if (status == NULL)
		throw std::logic_error("RawResponse constructor called with an unset response !");
	const std::map<std::string, std::string>	headers = response.getHeaderMap();
	const size_t								length = getFirstPartLength(headers, *status);

	std::string									firstPart;

	firstPart.reserve(length);
	firstPart.append(PROTOCOL)
		.append(" ")
		.append(status->getCodeStringRepresentation())
		.append(" ")
		.append(status->getText())
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
	const bool	hasBody = _body.isManagingValue() && _body.getValue() != NULL && _srcBodyFd.isManagingValue() && _srcBodyFd.getValue() != -1;
	if (_firstPartBuffer.getBufferLength() != 0)
	{
		const FlowState flowState = _firstPartBuffer.redirectBufferContentToFd(socketFd);

		if (flowState == FLOW_DONE)
			return (hasBody ? FLOW_MORE : FLOW_DONE);
		return (flowState);
	}
	if (hasBody == false)
		return (FLOW_DONE);
	if (_isBlocking == false)
		return (_bodyBuffer.redirectContent<int, ABody&>(_srcBodyFd.getValue(), *_body.getValue(), ABody::callInstanceWriteToFd));
	return (_bodyBuffer.redirectBufferContentToFd<ABody&>(*_body.getValue(), ABody::callInstanceWriteToFd));
}
