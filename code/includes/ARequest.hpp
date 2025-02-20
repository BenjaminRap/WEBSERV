#include "ServerConfiguration.hpp"

class ARequest
{
	protected :
		EMethods					_method; // Method of the request
		const ServerConfiguration	*_config; // Configuration of the server
		const Route					*_root; // Route of the request
		std::string					_url; // Url of the request
		bool						_isRoot; // Is the request at the root of the server

		int							_code;
		std::string					_statusText;
		std::string					_file;

		ARequest();

	public :
		explicit ARequest(std::string url, const ServerConfiguration&config, EMethods method);
		virtual ~ARequest() = 0;
		ARequest(const ARequest& src);
		ARequest& operator=(const ARequest& src);

		void					setResponse(int code, const std::string &status, const std::string& file);
		void					setUrl(const std::string& src);
		void					setRoot(const Route *root);
		void					setMethod(EMethods method);
		void					setIsRoot(bool src);

		std::string				&getUrl();
		const std::string		&getStatusText() const;
		const std::string		&getFile() const;
		const Route				*getRoot() const;
		int						getCode() const;
		EMethods				getMethod() const;
		bool					getIsRoot() const;
};