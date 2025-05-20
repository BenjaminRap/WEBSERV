#include "RequestContext.hpp"

RequestContext::~RequestContext(void)
{
}

RequestContext::RequestContext
(
	const  Host& host,
	const sockaddr_in clientAddr,
	Request& request,
	Response& response,
	EPollHandler& ePollHandler,
	FlowBuffer& requestBuffer,
	FlowBuffer& responseBuffer,
	ConnectedSocketData& connectedSocketData
) :
	host(host),
	clientAddr(clientAddr),
	request(request),
	response(response),
	ePollHandler(ePollHandler),
	requestBuff(requestBuffer),
	responseBuff(responseBuffer),
	connectedSocketData(connectedSocketData)
{

}
