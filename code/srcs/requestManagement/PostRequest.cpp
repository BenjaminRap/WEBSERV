#include <string>                 // for string, basic_string
#include <vector>                 // for vector

#include "ARequestType.hpp"       // for ARequestType, DIRE, LS_FILE
#include "EMethods.hpp"           // for EMethods
#include "PostRequest.hpp"        // for PostRequest
#include "requestStatusCode.hpp"  // for HTTP_METHOD_NOT_ALLOWED, HTTP_FORBI...

class RequestContext;  // lines 8-8
class ServerConfiguration;  // lines 9-9

bool		findIndex(ARequestType& req, const std::vector<std::string> &indexs);
void		directoryCase(PostRequest &post);

PostRequest::PostRequest
(
	std::string url,
	const ServerConfiguration &config,
	const std::string& domain,
	RequestContext& requestContext
) :
	ARequestType(url, config, POST, domain, requestContext)
{
	if (this->_code != 0)
		return ;

	if (_targetType == DIRE)
		directoryCase(*this);
	else if (_targetType == LS_FILE)
		setResponse(HTTP_METHOD_NOT_ALLOWED);
	else
		setResponse(_targetType);
}

PostRequest::~PostRequest()
{
}


void	directoryCase(PostRequest &post)
{
	if (findIndex(post, post.getIndexs()))
	{
		if (post.getCode() == HTTP_OK)
			post.setResponse(HTTP_METHOD_NOT_ALLOWED);
		return ;
	}
	post.setResponse(HTTP_FORBIDDEN);
}
