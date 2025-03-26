#ifndef GET_REQUEST_HPP
# define GET_REQUEST_HPP

# include <string>				// for std::string

# include "ARequestType.hpp"	// for ARequestType

class ServerConfiguration;

class GetRequest : public ARequestType
{
	private :
		GetRequest();
		GetRequest(const GetRequest& src);
		GetRequest& operator=(const GetRequest& src);

	public :
		explicit GetRequest(std::string url, const ServerConfiguration &config);
		~GetRequest();
};

#endif //!GET_REQUEST_HPP
