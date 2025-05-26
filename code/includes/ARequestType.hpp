#ifndef A_REQUEST_HPP
# define A_REQUEST_HPP

# include <sys/types.h>				// for size_t
# include <stdint.h>				// for uint16_t

# include "RequestContext.hpp"		// for RequestContext
# include "ServerConfiguration.hpp"	// for ServerConfiguration
# include "SharedResource.hpp"		// for SharedResource

# define DIRE 1
# define LS_FILE 2

class	AFdData;
class	EPollHandler;

/**
 * @class ARequestType
 * @brief The parent class of all request : GET, POST, PUT, DELETE ...
 *
 */
class ARequestType
{
	private:

		ARequestType();
		ARequestType(const ARequestType& src);
		ARequestType& operator=(const ARequestType& src);

	protected :
		EMethods					_method;
		const ServerConfiguration&	_config;
		const Route*				_route;
		std::string					_path;
		std::string					_domain;

		int							_code;
		std::string					_redirection;
		std::string					_autoIndexPage;
		std::string					_url;
		std::string					_queryString;
		SharedResource<AFdData*>	_inFd;
		SharedResource<AFdData*>	_outFd;

	public :
		explicit ARequestType
		(
			std::string &url,
			const ServerConfiguration& config,
			EMethods method,
			const std::string& domain,
			RequestContext& requestContext
		);
		virtual ~ARequestType() = 0;

		void									setResponseWithLocation(uint16_t code, const std::string &redirection, bool isReelRedirect);
		void									setResponse(uint16_t code);
		void									setPath(const std::string &src);
		void									setRoute(const Route *root);
		void									setMethod(EMethods method);
		void									setUrl(const std::string &url);

		const std::string&						getAutoIndexPage(void) const;
		bool									getAutoIndex(void) const;
		const std::vector<std::string>&			getIndexs(void) const;
		const std::vector<EMethods>&			getAcceptedMethods(void) const;
		const std::string&						getCgiFileExtension(void) const;
		const std::string&						getCgiInterpreter(void) const;
		const std::string&						getRoot(void) const;
		size_t									getMaxClientBodySize(void) const;
		const std::map<uint16_t, std::string>&	getErrorPages(void) const;
		std::string&							getPath(void);
		const std::string&						getRedirection(void) const;
		const Route*							getRoute(void) const;
		int										getCode(void) const;
		EMethods								getMethod(void) const;
		const SharedResource<AFdData*>&			getInFd(void) const;
		const SharedResource<AFdData*>&			getOutFd(void) const;
		const ServerConfiguration&				getConfig(void) const;
		std::string&							getUrl(void);
		uint16_t								setCgiAFdData(RequestContext& requestContext, const std::string& extension);
};

#endif //!A_REQUEST_HPP
