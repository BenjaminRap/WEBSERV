#ifndef REQUEST_CONTEXT_HPP
# define REQUEST_CONTEXT_HPP

# include <netinet/in.h>

# include "Host.hpp"

class	Request;
class	Response;
class	EPollHandler;
class	FlowBuffer;
class	ConnectedSocketData;

class	RequestContext {
private:
	RequestContext(void);
	RequestContext(const RequestContext &ref);
	RequestContext &operator=(const RequestContext &ref);
	
	const Host&					_host;
	const union sockaddr_in_u	_clientAddr;

	Request&					_request;
	Response&					_response;
	EPollHandler&				_ePollHandler;
	FlowBuffer&					_requestBuff;
	FlowBuffer&					_responseBuff;
	ConnectedSocketData&		_connectedSocketData;

	friend class	ARequestType;
public:

	~RequestContext(void)
	{
	}

	RequestContext
	(
		const Host& host,
		const sockaddr_in_u clientAddr,
		Request& request,
		Response& response,
		EPollHandler& ePollHandler,
		FlowBuffer& requestBuffer,
		FlowBuffer& responseBuffer,
		ConnectedSocketData& connectedSocketData
	) :
		_host(host),
		_clientAddr(clientAddr),
		_request(request),
		_response(response),
		_ePollHandler(ePollHandler),
		_requestBuff(requestBuffer),
		_responseBuff(responseBuffer),
		_connectedSocketData(connectedSocketData)
	{

	}
};

#endif // !REQUEST_CONTEXT_HPP
