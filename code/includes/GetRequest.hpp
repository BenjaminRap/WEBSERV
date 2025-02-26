#ifndef GETREQUEST_HPP
# define GETREQUEST_HPP

#include <string>
#include "ServerConfiguration.hpp"
#include "ARequest.hpp"

class GetRequest : public ARequest
{
	private :
		bool				_autoIndex;
		int					_index;

		GetRequest();
		GetRequest(const GetRequest& src);
		GetRequest& operator=(const GetRequest& src);

	public :
		explicit GetRequest(std::string url, const ServerConfiguration &config);
		~GetRequest();

		bool						getAutoIndex() const;
		void						setAutoIndex(bool src);
		std::vector<std::string>	getIndexVec();
};

#endif
