#ifndef PUT_REQUEST_HPP
# define PUT_REQUEST_HPP

# include <string>            // for string, basic_string

# include "ARequestType.hpp"  // for ARequestType

class ServerConfiguration;

class PutRequest : public ARequestType
{
	private :
		std::string _fileName;

		PutRequest();
		PutRequest(const PutRequest& src);
		PutRequest& operator=(const PutRequest& src);

	public :
		explicit PutRequest(std::string url, const ServerConfiguration &config);
		~PutRequest();
};

#endif // !PUT_REQUEST_HPP
