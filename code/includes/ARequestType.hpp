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
		std::string					_autoIndexPage;
		SharedResource<int>			_inFd;
		SharedResource<int>			_outFd;
		size_t						_outSize;

	public :
		explicit ARequestType(std::string &url, const ServerConfiguration& config, EMethods method);
		virtual ~ARequestType() = 0;

		void									setRedirectionResponse(uint16_t code, const std::string &redirection);
		void									setResponse(uint16_t code);
		void									setUrl(const std::string &src);
		void									setRoute(const Route *root);
		void									setMethod(EMethods method);

		const std::string&						getAutoIndexPage(void) const;
		bool									getAutoIndex(void) const;
		const std::vector<std::string>&			getIndexs(void) const;
		const std::map<uint16_t, std::string>&	getErrorPages(void) const;
		std::string&							getUrl(void);
		const std::string&						getRedirection(void) const;
		const Route*							getRoute(void) const;
		int										getCode(void) const;
		EMethods								getMethod(void) const;
		SharedResource<int>						getInFd(void) const;
		SharedResource<int>						getOutFd(void) const;
		size_t									getOutSize(void) const;
		const ServerConfiguration&				getConfig(void) const;
};

#endif //!A_REQUEST_HPP
