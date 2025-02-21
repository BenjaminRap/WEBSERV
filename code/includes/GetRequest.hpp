#ifndef GETREQUEST_HPP
# define GETREQUEST_HPP

#include <string>
#include "ServerConfiguration.hpp"
#include "ARequest.hpp"

class GetRequest : public ARequest
{
	private :
		GetRequest();

		bool				_autoIndex;
		int					_index;
	public :
		explicit GetRequest(std::string url, const ServerConfiguration &config);
		GetRequest(const GetRequest& src);
		~GetRequest();
		GetRequest& operator=(const GetRequest& src);

		bool						getAutoIndex() const;
		void						setAutoIndex(bool src);
		std::vector<std::string>	getIndexVec();
};

#endif