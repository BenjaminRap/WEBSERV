#ifndef GET_REQUEST_HPP
# define GET_REQUEST_HPP

# include <string>				// for std::string
# include <vector>				// for std::vector

# include "ARequestType.hpp"	// for ARequestType

class ServerConfiguration;

class GetRequest : public ARequestType
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

		bool							getAutoIndex() const;
		void							setAutoIndex(bool src);
		const std::vector<std::string>	&getIndexVec();
		const std::vector<std::string>	&getDefaultIndexVec();
};

#endif //!GET_REQUEST_HPP
