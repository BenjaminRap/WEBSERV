#include <cstring>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>

#include "ServerConfiguration.hpp"
#include "ARequestType.hpp"

class PutRequest : public ARequestType
{
	private :
		int			_fd;
		std::string _fileName;

		PutRequest();
		PutRequest(const PutRequest& src);
		PutRequest& operator=(const PutRequest& src);

	public :
		explicit PutRequest(std::string url, const ServerConfiguration &config);
		~PutRequest();

		int getFd(void) const;

		void setFd(int fd);
};
