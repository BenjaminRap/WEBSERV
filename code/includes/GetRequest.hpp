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
		int					_index;
		bool				_isRoot;
	public :
		explicit GetRequest(const std::string& url, ServerConfiguration config);
		GetRequest(const GetRequest& src);
		~GetRequest();
		GetRequest& operator=(const GetRequest& src);

		void						setResponse(int code, const std::string& file);
		void						setUrl(const std::string& src);
		void						setAutoIndex(bool src);
		void						setRoot(Route *root);
		void						setIsRoot(bool src);
		bool						getAutoIndex() const;
		bool						getIsRoot() const;
		std::vector<std::string>	getIndexVec();
		std::string					getUrl();

		/**
		* @brief This Variable represent the code of the request, 200 : all good, 403 : Forbiden etc ...
		*/
		int			code;
		/**
		* @brief This Variable represent the response of the request, it could be a path, or content of a file, show code to know
		*/
		std::string		file;
};
