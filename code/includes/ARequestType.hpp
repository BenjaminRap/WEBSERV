#ifndef AREQUEST_HPP
# define AREQUEST_HPP

#include "ServerConfiguration.hpp"

#define DIRE 1
#define LS_FILE 2
#define NF 3

#define FORBIDEN -1
#define ERROR500 -2

class ARequestType
{
	private:
		static const std::map<int, std::string>	_statuses;

		ARequestType();
		ARequestType(const ARequestType& src);
		ARequestType& operator=(const ARequestType& src);

	protected :
		EMethods					_method;
		const ServerConfiguration	*_config;
		const Route					*_route;
		std::string					_url;
		bool						_isRoute;

		int							_code;
		std::string					_redirection;

	public :
		explicit ARequestType(std::string &url, const ServerConfiguration&config, EMethods method);
		virtual ~ARequestType() = 0;

		static const std::string	&getStatusText(int code);

		void						setRedirectionResponse(int code, const std::string &redirection);
		void						setResponse(int code);
		void						setUrl(const std::string &src);
		void						setRoute(const Route *root);
		void						setMethod(EMethods method);
		void						setIsRoute(bool src);

		std::string					&getUrl();
		const std::string			&getRedirection() const;
		const Route					*getRoute() const;
		int							getCode() const;
		EMethods					getMethod() const;
		bool						getIsRoute() const;

		const std::string			&getError(unsigned short error);
};

#endif
