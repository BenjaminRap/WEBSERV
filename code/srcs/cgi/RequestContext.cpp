#include "RequestContext.hpp"

RequestContext::~RequestContext(void)
{
}

RequestContext::RequestContext
(
		Request& request,
		Response& response,
		EPollHandler& ePollHandler,
		FlowBuffer& requestBuffer,
		FlowBuffer& responseBuffer,
		ConnectedSocketData& connectedSocketData
) :
	request(request),
	response(response),
	ePollHandler(ePollHandler),
	requestBuff(requestBuffer),
	responseBuff(responseBuffer),
	connectedSocketData(connectedSocketData)
{

}
