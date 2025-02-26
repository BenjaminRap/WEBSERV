#ifndef AREQUEST_HPP
# define AREQUEST_HPP

#include "ServerConfiguration.hpp"

#define DIRE 1
#define FILE 2
#define NF 3

#define FORBIDEN -1
#define ERROR500 -2

class ARequest
{
	private:
		ARequest();
		ARequest(const ARequest& src);
		ARequest& operator=(const ARequest& src);

	protected :
		EMethods					_method;
		const ServerConfiguration	*_config;
		const Route					*_route;
		std::string					_url;
		bool						_isRoot;

		int							_code;
		std::string					_statusText;
		std::string					_file;

	public :
		explicit ARequest(std::string &url, const ServerConfiguration&config, EMethods method);
		virtual ~ARequest() = 0;

		void					setResponse(int code, const std::string &status, const std::string &file);
		void					setUrl(const std::string &src);
		void					setRoute(const Route *root);
		void					setMethod(EMethods method);
		void					setIsRoot(bool src);

		std::string				&getUrl();
		const std::string		&getStatusText() const;
		const std::string		&getFile() const;
		const Route				*getRoute() const;
		int						getCode() const;
		EMethods				getMethod() const;
		bool					getIsRoot() const;

		const std::string		&getError(unsigned short error);
};

#endif
