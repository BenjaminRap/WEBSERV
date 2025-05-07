#ifndef A_REQUEST_HPP
# define A_REQUEST_HPP

# include <sys/types.h>				// for size_t
# include <stdint.h>				// for uint16_t

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
		EPollHandler&				_ePollHandler;
		const Route*				_route;
		std::string					_url;
		std::string					_domain;

		int							_code;
		std::string					_redirection;
		std::string					_autoIndexPage;
		std::string					_backupUrl;
		SharedResource<AFdData*>	_inFd;
		SharedResource<AFdData*>	_outFd;

	public :
		explicit ARequestType
		(
			std::string &url,
			const ServerConfiguration& config,
			EPollHandler& ePollHandler,
			EMethods method,
			const std::string& domain
		);
		virtual ~ARequestType() = 0;

		void									setRedirectionResponse(uint16_t code, const std::string &redirection, bool isReelRedirect);
		void									setResponse(uint16_t code);
		void									setUrl(const std::string &src);
		void									setRoute(const Route *root);
		void									setMethod(EMethods method);
		void									setBackupUrl(const std::string &url);

		const std::string&						getAutoIndexPage(void) const;
		bool									getAutoIndex(void) const;
		const std::vector<std::string>&			getIndexs(void) const;
		const std::map<uint16_t, std::string>&	getErrorPages(void) const;
		std::string&							getUrl(void);
		const std::string&						getRedirection(void) const;
		const Route*							getRoute(void) const;
		int										getCode(void) const;
		EMethods								getMethod(void) const;
		SharedResource<AFdData*>				getInFd(void) const;
		SharedResource<AFdData*>				getOutFd(void) const;
		const ServerConfiguration&				getConfig(void) const;
		std::string&							getBackupUrl(void);
		int										execCGI(const char *path, char **argv, char **env, int fd[2]);
};

#endif //!A_REQUEST_HPP
