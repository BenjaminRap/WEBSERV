#ifndef A_REQUEST_TYPE_HPP
# define A_REQUEST_TYPE_HPP

# include <sys/types.h>				// for size_t
# include <stdint.h>				// for uint16_t

# include "RequestContext.hpp"		// for RequestContext
# include "ServerConfiguration.hpp"	// for ServerConfiguration
# include "SharedResource.hpp"		// for SharedResource

# define DIRE 1
# define LS_FILE 2

class	AFdData;
class	EPollHandler;
class	CgiOutArgs;
class	SizedBody;
class	ChunkedBody;

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
		const ServerConfiguration&	_config;
		const Route*				_route;
		/**
		 * @brief The real path to the file/directory.
		 * Meaning the path on the computer, relative to the
		 * program path.
		 */
		std::string					_path;
		std::string					_domain;
		int							_code;
		std::string					_redirection;
		std::string					_autoIndexPage;
		/**
		 * @brief The client target without the queryString.
		 * @ex /get?truc=bidule
		 * url: /get
		 */
		std::string					_url;
		/**
		 * @brief Corresponds to the cgi QUERY_STRING variable.
		 * @ex /get?truc=bidule
		 * queryString : truc=bidule
		 */
		std::string					_queryString;
		/**
		 * @brief Corresponds to the  cgi PATH_INFO variable.
		 * @ex : /file.cgi/path/info
		 * pathInfo : /path/info
		 */
		std::string					_pathInfo;
		SharedResource<AFdData*>	_inFd;
		SharedResource<AFdData*>	_outFd;
		/**
		 * @brief Either the type of node : FILE , DIRE, or an HTTP error
		 * if we can't get the type.
		 */
		uint16_t					_targetType;
		bool						_isCgi;

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

		/********************** Setters ***********************************/

		/**
		 * @brief Set the response and the location value, added later in the location header.
		 *
		 * @param redirection If isReelRedirect is not true, it will use it to create the location
		 * header.
		 * @param isReelRedirect Is the redirection setup in the configuration.
		 * @throw It can throw a std::bad_alloc
		 */
		void									setResponseWithLocation(uint16_t code, const std::string &redirection, bool isReelRedirect);
		void									setResponse(uint16_t code);
		void									setPath(const std::string &src);
		void									setRoute(const Route *root);
		void									setUrl(const std::string &url);
		void									setOutFd(AFdData* fdData, void (&free)(AFdData*));
		void									setInFd(AFdData* fdData, void (&free)(AFdData*));

		/************************** Getters *************************************/

		const std::map<uint16_t, std::string>&	getErrorPages(void) const;
		const std::string&						getAutoIndexPage(void) const;
		std::string&							getPath(void);
		const std::string&						getRedirection(void) const;
		const Route*							getRoute(void) const;
		int										getCode(void) const;
		const SharedResource<AFdData*>&			getInFd(void) const;
		const SharedResource<AFdData*>&			getOutFd(void) const;
		const ServerConfiguration&				getConfig(void) const;
		const std::string&						getUrl(void) const;
		const std::string&						getPath(void) const;
		const std::string&						getQueryString(void) const;
		const std::string&						getPathInfo(void) const;
		bool									getIsCgi(void) const;
		std::string&							getUrl(void);


		/**
		 * The nexts getters return the Route or ServerConfiguration version
		 * of the variables, depending on  : if there is a route, and is the option
		 * set in the route.
		 */
	

		/***/
		bool									getAutoIndex(void) const;
		const std::vector<std::string>&			getIndexs(void) const;
		const std::vector<EMethods>&			getAcceptedMethods(void) const;
		const std::string&						getCgiFileExtension(void) const;
		const std::string&						getCgiInterpreter(void) const;
		const std::string&						getRoot(void) const;
		size_t									getMaxClientBodySize(void) const;
		const std::list<ConfigHeaders>&			getAddHeader(void) const;

		/************************Cgi Methods***********************************/

		uint16_t								setCgiAFdData(RequestContext& requestContext);
		/**
		 * @brief Set the _cgiIn managed resource to a new instance of the CgiIn class, instanciated
		 * with its ChunkedBody constructor. If this method is called setCgiOut should not be called.
		 * If the CgiIn instance is created successfully, env and argv are emptied, and cgiOutArgs
		 * is set to NULL, the CgiIn will own them.
		 *
		 * @throw It can throw a std::bad_alloc, or a std::exception if we can't add
		 * the fd to the ePollHandler list.
		 */
		void									setCgiInChunked
												(
													RequestContext& requestContext,
													const char* (&env)[23],
													const char* (&argv)[3],
													const CgiOutArgs*& cgiOutArgs,
													ChunkedBody& body
												);
		/**
		 * @brief Set the _cgiIn managed resource to a new instance of the CgiIn class, instanciated
		 * with its SizedBody constructor. If this method is called setCgiOut should be called.
		 *
		 * @throw It can throw a std::bad_alloc, or a std::exception if we can't add
		 * the fd to the ePollHandler list.
		 */
		void									setCgiInSized
												(
													RequestContext& requestContext,
													int& fd,
													SizedBody& body
												);
		/**
		 * @brief Set the _cgiOut managed resource to a new instance of the CgiOut class.
		 * If the CgiOut instance is created successfully, fd and pid are set to -1, the
		 * CgiOut will own them.
		 *
		 * @throw It can throw a std::bad_alloc, or a std::exception if we can't add
		 * the fd to the ePollHandler list.
		 */
		void									setCgiOut
												(
													RequestContext& requestContext,
													int& fd,
													pid_t& pid,
													const CgiOutArgs& cgiOutArgs
												);
		bool									setPathInfo(const std::string& extension, std::string path);
		void									setEnv(const char *(&env)[23], RequestContext& requestContext);
};

#endif //!A_REQUEST_TYPE_HPP
