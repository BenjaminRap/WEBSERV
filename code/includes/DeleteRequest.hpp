#include <string>
#include <list>
#include "ServerConfiguration.hpp"
#include "ARequest.hpp"

class DeleteRequest : public ARequest
{
	private :
		DeleteRequest();

	public :
		explicit DeleteRequest(std::string url, const ServerConfiguration &config);
		DeleteRequest(const DeleteRequest& src);
		~DeleteRequest();
		DeleteRequest& operator=(const DeleteRequest& src);
};
