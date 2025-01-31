#include <string>
#include <list>
#include "ServerConfiguration.hpp"

class DeleteRequest
{
	private :
		DeleteRequest();

		const ServerConfiguration	*_config;
		const Route					*_root;
		std::string					_url;
		bool						_isRoot;
	public :
		explicit DeleteRequest(std::string url, const ServerConfiguration &config);
		DeleteRequest(const DeleteRequest& src);
		~DeleteRequest();
		DeleteRequest& operator=(const DeleteRequest& src);

		void						parsing(std::string &url, const ServerConfiguration &config);
		void						setResponse(int newcode, const std::string &status, const std::string& newfile);
		void						setUrl(const std::string& src);
		void						setRoot(const Route *root);
		void						setIsRoot(bool src);
		bool 						getIsRoot() const;

		std::string					&getUrl();
		std::string					getError(unsigned short error);


	/**
		* @brief This Variable represent the code of the request, 200 : all good, 403 : Forbiden etc ...
		*/
		int			code;

		std::string statusText;

		/**
		* @brief This Variable represent the response of the request, it could be a path, or content of a file, show code to know
		*/
		std::string		file;
};
