#include <stddef.h>            // for size_t, NULL
#include <map>                 // for map, _Rb_tree_const_iterator, operator!=
#include <stdexcept>           // for logic_error
#include <string>              // for basic_string, string
#include <utility>             // for pair

#include "ABody.hpp"           // for ABody
#include "FlowBuffer.hpp"      // for FlowState, FlowBuffer
#include "RawResponse.hpp"     // for RawResponse, LINE_END, LINE_END_LENGTH
#include "Response.hpp"        // for Response
#include "SharedResource.hpp"  // for SharedResource
#include "Status.hpp"          // for Status, StatusType
#include "protocol.hpp"        // for PROTOCOL, PROTOCOL_LENGTH

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

RawResponse::~RawResponse()
{
}

/*******************************Member functions*******************************/

size_t	getFirstPartLength
(
	const std::map<std::string, std::string>& headers,
	const Status& status,
	size_t autoIndexPageSize
)
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
	length += status.getErrorPage().size();
	if (status.isOfType(STATUS_SUCESSFULL))
		length += autoIndexPageSize;
	length += 1; // for the /0
	return (length);
}

std::string	getFirstPart(const Response &response)
{
	const Status * const						status = response.getStatus();

	if (status == NULL)
		throw std::logic_error("RawResponse constructor called with an unset response !");
	const std::map<std::string, std::string>	headers = response.getHeaderMap();
	const std::string&							autoIndexPage = response.getAutoIndexPage();
	const size_t								length = getFirstPartLength(headers, *status, autoIndexPage.size());

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
	firstPart.append(status->getErrorPage());
	if (status->isOfType(STATUS_SUCESSFULL))
		firstPart.append(autoIndexPage);
	return (firstPart);
}

FlowState	RawResponse::sendResponseToSocket(int socketFd)
{
	const bool	hasBody = _body.isManagingValue() && _body.getValue() != NULL && _srcBodyFd.isManagingValue() && _srcBodyFd.getValue() != -1;
	if (_firstPartBuffer.getContentLength() != 0)
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
