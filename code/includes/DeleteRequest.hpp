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
		explicit DeleteRequest(std::string url, const ServerConfiguration &config);
		~DeleteRequest();
};
