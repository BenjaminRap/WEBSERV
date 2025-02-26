#include <string>
#include <list>
#include "ServerConfiguration.hpp"
#include "ARequest.hpp"

class DeleteRequest : public ARequest
{
	private :
		DeleteRequest();
		DeleteRequest(const DeleteRequest& src);

	public :
		explicit DeleteRequest(std::string url, const ServerConfiguration &config);
		~DeleteRequest();
		DeleteRequest& operator=(const DeleteRequest& src);
};
