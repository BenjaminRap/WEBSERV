#ifndef DELETE_REQUEST_HPP
# define DELETE_REQUEST_HPP

# include <string>            // for string

# include "ARequestType.hpp"  // for ARequestType

class ServerConfiguration;

class DeleteRequest : public ARequestType
{
	private :
		DeleteRequest();
		DeleteRequest(const DeleteRequest& src);
		DeleteRequest& operator=(const DeleteRequest& src);

	public :
		explicit DeleteRequest(std::string &url, std::string &domain, const ServerConfiguration &config);
		~DeleteRequest();
};

#endif // !DELETE_REQUEST_HPP
