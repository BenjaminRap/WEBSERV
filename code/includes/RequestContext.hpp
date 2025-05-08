#ifndef REQUEST_CONTEXT_HPP
# define REQUEST_CONTEXT_HPP

class	Request;
class	Response;
class	EPollHandler;
class	FlowBuffer;
class	ConnectedSocketData;

class RequestContext {
private:
	RequestContext(void);
	RequestContext(const RequestContext &ref);
	RequestContext &operator=(const RequestContext &ref);
	
public:
	const Request&			request;
	Response&				response;
	EPollHandler&			ePollHandler;
	FlowBuffer&				requestBuff;
	FlowBuffer&				responseBuff;
	ConnectedSocketData&	connectedSocketData;

	~RequestContext(void);
	RequestContext
	(
		const Request& request,
		Response& response,
		EPollHandler& ePollHandler,
		FlowBuffer& requestBuffer,
		FlowBuffer& responseBuffer,
		ConnectedSocketData& connectedSocketData
	);
};

#endif // !REQUEST_CONTEXT_HPP
