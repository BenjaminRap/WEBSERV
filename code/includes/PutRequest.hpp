#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>

#include "ServerConfiguration.hpp"
#include "ARequest.hpp"

class PutRequest : public ARequest
{
	private :
		int			_fd;

		PutRequest();
		PutRequest(const PutRequest& src);
		PutRequest& operator=(const PutRequest& src);

	public :
		explicit PutRequest(std::string url, std::string fileName, const ServerConfiguration &config);
		~PutRequest();

		int getFd(void) const;

		void setFd(int fd);
};
