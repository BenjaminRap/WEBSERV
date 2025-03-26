#ifndef A_REQUEST_HPP
# define A_REQUEST_HPP

# include <sys/types.h>				// for size_t
# include <stdint.h>				// for uint16_t

# include "ServerConfiguration.hpp"	// for ServerConfiguration
# include "SharedResource.hpp"		// for SharedResource

# define DIRE 1
# define LS_FILE 2
# define NF 3

class ARequestType
{
	private:
		static const std::map<int, std::string>	_statuses;

		ARequestType();
		ARequestType(const ARequestType& src);
		ARequestType& operator=(const ARequestType& src);

	protected :
		EMethods					_method;
		const ServerConfiguration	&_config;
		const Route					*_route;
		std::string					_url;

		int							_code;
		std::string					_redirection;
		SharedResource<int>			_inFd;
		SharedResource<int>			_outFd;
		size_t						_outSize;

	public :
		explicit ARequestType(std::string &url, const ServerConfiguration& config, EMethods method);
		virtual ~ARequestType() = 0;

		static const std::string&				getStatusText(int code);

		void									setRedirectionResponse(int code, const std::string &redirection);
		void									setResponse(int code);
		void									setUrl(const std::string &src);
		void									setRoute(const Route *root);
		void									setMethod(EMethods method);

		bool									getAutoIndex(void) const;
		const std::vector<std::string>&			getIndexs(void) const;

		std::string&							getUrl();
		const std::string&						getRedirection() const;
		const Route*							getRoute() const;
		int										getCode() const;
		EMethods								getMethod() const;
		SharedResource<int>						getInFd();
		SharedResource<int>						getOutFd();
		size_t									getOutSize() const;

		const std::string			&getError(unsigned short error);
};

#endif //!A_REQUEST_HPP
