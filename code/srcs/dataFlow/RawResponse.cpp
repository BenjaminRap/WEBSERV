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


void	setFirstPart(std::string& result, const Status& status, const std::string& autoIndexPage, const Headers& headers, bool hasBody);

RawResponse::RawResponse(Response &response, FlowBuffer &bodyBuffer) :
	_firstPart(),
	_firstPartBuffer(NULL, 0, 0),
	_fdData(response.getFdData()),
	_flowBuf(bodyBuffer)
{
	const Status * const		status = response.getStatus();

	if (status == NULL)
		throw std::logic_error("RawResponse constructor called with an unset response !");
	if (_fdData.isManagingValue())
	{
		AFdData* fdData = _fdData.getValue();

		if (fdData->getType() == CGI_OUT)
			return ;
	}
	setFirstPart(_firstPart, *status, response.getAutoIndexPage(), response.getHeaders(), _fdData.isManagingValue());
	_firstPartBuffer.setBuffer(&_firstPart[0], _firstPart.size(), _firstPart.capacity());
}

RawResponse::~RawResponse()
{
}

/*******************************Member functions*******************************/

static size_t	getFirstPartLength
(
	const Headers& headers,
	const Status& status,
	size_t autoIndexPageSize,
	bool hasBody
)
{
	size_t										length = 0;

	length += status.getRepresentationSize();
	length += headers.getTotalSize();
	length += LINE_END_LENGTH; // for the empty line
	length += 1; // for the /0
	if (hasBody)
		return (length);
	if (status.isOfType(STATUS_SUCESSFULL))
		length += autoIndexPageSize;
	else if (status.isOfType(STATUS_ERROR))
		length += status.getErrorPage().size();
	return (length);
}

void	setFirstPart
(
	std::string& result,
	const Status& status,
	const std::string& autoIndexPage,
	const Headers& headers,
	bool hasBody
)
{
	const size_t								length = getFirstPartLength(headers, status, autoIndexPage.size(), hasBody);

	result.reserve(length);
	result += status.getRepresentation();
	result += headers;
	result.append(LINE_END);
	if (hasBody)
		return ;
	if (status.isOfType(STATUS_SUCESSFULL))
		result.append(autoIndexPage);
	else if (status.isOfType(STATUS_ERROR))
		result.append(status.getErrorPage());
}

FlowState	RawResponse::sendResponseToSocket(int socketFd)
{
	const bool	hasBody = _fdData.isManagingValue();

	if (_firstPartBuffer.getContentLength() != _firstPartBuffer.getNumCharsWritten())
	{
		const FlowState flowState = _firstPartBuffer.buffToDest(socketFd);

		if (flowState == FLOW_DONE)
			return (hasBody ? FLOW_MORE : FLOW_DONE);
		return (flowState);
	}
	if (hasBody == false)
		return (FLOW_DONE);
	AFdData * const	fdData = _fdData.getValue();

	fdData->callback(0);
	if (fdData->getIsBlocking())
	{
		const FlowState flowState = _flowBuf.buffToDest(socketFd);

		if (!fdData->getIsActive() && flowState == FLOW_DONE)
			return (FLOW_MORE);
		return (flowState);
	}
	else
		return (_flowBuf.redirect(fdData->getFd(), socketFd));
}
