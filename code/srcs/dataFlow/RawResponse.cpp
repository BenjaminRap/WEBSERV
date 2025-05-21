#include <stddef.h>            // for NULL, size_t
#include <iostream>            // for basic_ostream, basic_ios, cout, endl
#include <iterator>            // for distance
#include <stdexcept>           // for logic_error
#include <string>              // for basic_string, char_traits, string, ope...

#include "AFdData.hpp"         // for AFdData, AFdDataChilds
#include "CgiOut.hpp"          // for CgiOut
#include "FlowBuffer.hpp"      // for FlowState, FlowBuffer
#include "Headers.hpp"         // for operator+=, Headers, LINE_END, LINE_EN...
#include "RawResponse.hpp"     // for RawResponse
#include "Response.hpp"        // for Response
#include "SharedResource.hpp"  // for SharedResource
#include "Status.hpp"          // for Status, StatusType

/*************************Constructors / Destructors***************************/


std::string&	getFirstPart(const Status& status, const Headers& headers, const char* bodyBegin, const char* bodyEnd);

RawResponse::RawResponse(Response &response, FlowBuffer &bodyBuffer) :
	_firstPart(),
	_firstPartBuffer(NULL, 0, 0),
	_fdData(response.getFdData()),
	_flowBuf(bodyBuffer)
{
	if (_fdData.isManagingValue()
		&& _fdData.getValue()->getType() == CGI_OUT)
	{
		return ;
	}
	const Status * const		status = response.getStatus();

	if (status == NULL)
		throw std::logic_error("RawResponse constructor called with an unset response !");
	const char*	bodyBegin;
	const char*	bodyEnd;

	if (_fdData.isManagingValue())
	{
		bodyBegin = NULL;
		bodyEnd = NULL;
	}
	else if (status->isOfType(STATUS_ERROR))
	{
		const std::string&	errorPage = status->getErrorPage();

		bodyBegin = errorPage.c_str();
		bodyEnd = bodyBegin + errorPage.size();
	}
	else
	{
		const std::string&	autoIndexPage = response.getAutoIndexPage();

		bodyBegin = autoIndexPage.c_str();
		bodyEnd = bodyBegin + autoIndexPage.size();
	}

	_firstPart = getFirstPart(*status, response.getHeaders(), bodyBegin, bodyEnd);
	_firstPartBuffer.setBuffer(&_firstPart[0], _firstPart.size(), _firstPart.capacity());
	std::cout << _firstPart << std::endl;
}

RawResponse::~RawResponse()
{
}

/*******************************Member functions*******************************/

static size_t	getFirstPartLength
(
	const Status& status,
	const Headers& headers,
	const char* bodyBegin,
	const char* bodyEnd
)
{
	size_t										length = 0;

	length += status.getRepresentationSize();
	length += headers.getTotalSize();
	length += LINE_END_LENGTH; // for the empty line
	length += 1; // for the /0
	if (bodyBegin != NULL && bodyEnd != NULL)
		length += std::distance(bodyBegin, bodyEnd);
	return (length);
}

std::string&	getFirstPart
(
	const Status& status,
	const Headers& headers,
	const char* bodyBegin,
	const char* bodyEnd
)
{
	static std::string	result;
	const size_t								length = getFirstPartLength(status, headers, bodyBegin, bodyEnd);

	result.clear();
	result.reserve(length);
	result += status.getRepresentation();
	result += headers;
	result.append(LINE_END);
	if (bodyBegin != NULL && bodyEnd != NULL)
		result.append(bodyBegin, bodyEnd);
	return (result);
}

bool	canWriteFromBuffer(const AFdData* fdData)
{
	if (fdData->getType() != CGI_OUT)
		return (true);
	const CgiOut * const	cgiOut = static_cast<const CgiOut*>(fdData);

	return (cgiOut->isResponseReady());
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
		if (!canWriteFromBuffer(fdData))
			return (FLOW_MORE);
		const FlowState flowState = _flowBuf.buffToDest(socketFd);

		if (fdData->getIsActive() && flowState == FLOW_DONE)
			return (FLOW_MORE);
		return (flowState);
	}
	else
		return (_flowBuf.redirect(fdData->getFd(), socketFd));
}
