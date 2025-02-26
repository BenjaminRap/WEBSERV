#include <string>
#include "ServerConfiguration.hpp"
#include "ARequestType.hpp"

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
