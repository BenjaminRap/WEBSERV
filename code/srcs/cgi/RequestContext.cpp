#include "RequestContext.hpp"

RequestContext::~RequestContext(void)
{
}

RequestContext::RequestContext
(
		const Request& request,
		Response& response,
		EPollHandler& ePollHandler,
		FlowBuffer& requestBuffer,
		FlowBuffer& responseBuffer,
		ConnectedSocketData& connectedSocketData
) :
	_request(request),
	_response(response),
	_ePollHandler(ePollHandler),
	_requestBuff(requestBuffer),
	_responseBuff(responseBuffer),
	_connectedSocketData(connectedSocketData)
{

}
