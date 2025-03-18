#ifndef GETREQUEST_HPP
# define GETREQUEST_HPP

#include <string>
#include <fcntl.h>
#include "ServerConfiguration.hpp"
#include "ARequestType.hpp"

class GetRequest : public ARequestType
{
	private :
		bool				_autoIndex;
		int					_index;
		int					_fd;

		GetRequest();
		GetRequest(const GetRequest& src);
		GetRequest& operator=(const GetRequest& src);

	public :
		explicit GetRequest(std::string url, const ServerConfiguration &config);
		~GetRequest();

		bool							getAutoIndex() const;
		void							setAutoIndex(bool src);
		const std::vector<std::string>	&getIndexVec();
		const std::vector<std::string>	&getDefaultIndexVec();

		int								getFd() const;
		void							setFd(int fd);
};

#endif
