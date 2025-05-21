#ifndef POST_REQUEST_HPP
# define POST_REQUEST_HPP

# include <string>            // for string, basic_string
# include "ARequestType.hpp"  // for ARequestType

class ServerConfiguration;

class PostRequest : public ARequestType
{
private :
	std::string _fileName;

	PostRequest();
	PostRequest(const PostRequest& src);
	PostRequest& operator=(const PostRequest& src);

public :
	explicit PostRequest
			(
					std::string url,
					const ServerConfiguration &config,
					const std::string& domain,
					RequestContext& requestContext
			);
	~PostRequest();
};

#endif
