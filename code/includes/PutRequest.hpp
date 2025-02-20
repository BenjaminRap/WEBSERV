#include <cstring>
#include <list>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <cerrno>

#include "ServerConfiguration.hpp"

class PutRequest
{
	private :
		PutRequest();

		const ServerConfiguration	*_config;
		const Route					*_root;
		std::string					_url;
		bool						_isRoot;
	public :
		explicit PutRequest(std::string url, std::string fileName, const ServerConfiguration &config);
		PutRequest(const PutRequest& src);
		~PutRequest();
		PutRequest& operator=(const PutRequest& src);

		void						parsing(std::string &url, const ServerConfiguration &config);
		void						setResponse(int newcode, const std::string &status, const std::string& newfile);
		void						setUrl(const std::string& src);
		void						setRoot(const Route *root);
		void						setIsRoot(bool src);
		bool 						getIsRoot() const;

		std::string					&getUrl();
		std::string					getError(unsigned short error);


		int				code;
		std::string		statusText;
		std::string		file;
		int				fd;
};
