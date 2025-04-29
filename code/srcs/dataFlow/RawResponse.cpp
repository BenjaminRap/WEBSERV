#include <stddef.h>            // for size_t, NULL
#include <stdexcept>           // for logic_error
#include <string>              // for basic_string, string

#include "ABody.hpp"           // for ABody
#include "FlowBuffer.hpp"      // for FlowState, FlowBuffer
#include "RawResponse.hpp"     // for RawResponse, LINE_END, LINE_END_LENGTH
#include "Response.hpp"        // for Response
#include "SharedResource.hpp"  // for SharedResource
#include "Status.hpp"          // for Status, StatusType

/*************************Constructors / Destructors***************************/

std::string	getFirstPart(const Response& response);

RawResponse::RawResponse(Response &response, FlowBuffer &bodyBuffer) :
	_firstPart(getFirstPart(response)),
	_firstPartBuffer(&_firstPart[0], _firstPart.capacity(), _firstPart.length()),
	_fdData(response.getFdData()), _body(response.getBody()),
	_flowBuf(bodyBuffer)
{
	
}

RawResponse::~RawResponse()
{
}

/*******************************Member functions*******************************/

static size_t	getFirstPartLength
(
	const Headers& headers,
	const Status& status,
	size_t autoIndexPageSize
)
{
	size_t										length = 0;

	length += status.getRepresentationSize();
	length += headers.getTotalSize();
	length += LINE_END_LENGTH; // for the empty line
	if (status.isOfType(STATUS_SUCESSFULL))
		length += autoIndexPageSize;
	else if (status.isOfType(STATUS_ERROR))
		length += status.getErrorPage().size();
	length += 1; // for the /0
	return (length);
}

void	setFirstPart
(
	std::string& result,
	const Status& status,
	const std::string& autoIndexPage,
	const Headers& headers
)
{
	const size_t								length = getFirstPartLength(headers, status, autoIndexPage.size());

	result.reserve(length);
	result += status.getRepresentation();
	result += headers;
	result.append(LINE_END);
	if (status.isOfType(STATUS_SUCESSFULL))
		result.append(autoIndexPage);
	else if (status.isOfType(STATUS_ERROR))
		result.append(status.getErrorPage());
}

std::string	getFirstPart(const Response &response)
{
	const Status * const		status = response.getStatus();

	if (status == NULL)
		throw std::logic_error("RawResponse constructor called with an unset response !");
	std::string	firstPart;
	
	setFirstPart(firstPart, *status, response.getAutoIndexPage(), response.getHeaders());
	return (firstPart);
}

FlowState	RawResponse::sendResponseToSocket(int socketFd)
{
	const bool	hasBody = _body.isManagingValue() && _fdData.isManagingValue();

	if (_firstPartBuffer.getContentLength() != 0)
	{
		const FlowState flowState = _firstPartBuffer.buffToDest(socketFd);

		if (flowState == FLOW_DONE)
			return (hasBody ? FLOW_MORE : FLOW_DONE);
		return (flowState);
	}
	if (hasBody == false)
		return (FLOW_DONE);
	AFdData * const	fdData = _fdData.getValue();
	ABody * const	body = _body.getValue();

	fdData->callback(0);
	if (fdData->getIsBlocking())
	{
		const FlowState flowState = _flowBuf.buffToDest<ABody&>(*body, ABody::writeToFd);

		if (flowState == FLOW_DONE)
			return (fdData->getIsActive() ? FLOW_MORE : FLOW_DONE);
		return (flowState);
	}
	else
		return (_flowBuf.redirect<int, ABody&>(fdData->getFd(), *body, ABody::writeToFd));
}
