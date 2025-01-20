#include <string>
#include "ServerConfiguration.hpp"
/**
 * This class treat a GET request. The construtor do everthing and the result is store in :
 * - std::string file
 * - int code.
 */

class GetRequest
{
	private :
		GetRequest();

		ServerConfiguration	*_config;
		Route				*_root;
		std::string			_url;
		bool				_autoIndex;
		bool				_isDirectory;
		int					_index;
	public :
		explicit GetRequest(const std::string& url, ServerConfiguration config);
		GetRequest(const GetRequest& src);
		~GetRequest();
		GetRequest& operator=(const GetRequest& src);

		void setResponse(int code, const std::string& file);
		void setIsDirectory(bool src);
		bool getIsDirectory() const;
		void setUrl(const std::string& src);
		std::string getUrl();
		void setAutoIndex(bool src);
		bool getAutoIndex() const;
		std::vector<std::string> getIndexVec();
		void	setRoot(Route *root);
		Route	*getRoot() const;


	/**
		* @brief This Variable represent the code of the request, 200 : all good, 403 : Forbiden etc ...
		*/
		int			code;
		/**
		* @brief This Variable represent the response of the request, it could be a path, or content of a file, show code to know
		*/
		std::string	file;
};
