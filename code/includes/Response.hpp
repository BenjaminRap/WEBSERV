#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <stdint.h>  				// for uint16_t
# include <iostream>  				// for ostream
# include <map>       				// for map
# include <string>    				// for string, basic_string

# include "ABody.hpp"				// for ABody
# include "ServerConfiguration.hpp"	// for ServerConfiguration
# include "SharedResource.hpp"		// for SharedResource

class Status;

class	ARequestType;

/**
 * @brief The classe that stores all the data that will be sent to the client.
 * This class should be converted into a RawResponse to be used.
 * The creation of this class is not performant but displays the response variable
 * more clearly.
 * This class takes responsability for closing the fd.
 * The use of this class isn't mandatory because it has few uses and costs performances.
 */
class Response
{
private:
	const Status*						_status;
	std::map<std::string, std::string>	_headers;
	/**
	 * @brief The file descriptor of the body.If there is no body, this variable
	 * is set to -1.
	 */
	SharedResource<int>					_bodySrcFd;
	bool								_isBlocking;
	SharedResource<ABody*>				_body;
	const ServerConfiguration&			_defaultConfig;
	std::string							_autoIndexPage;

	Response(void);
	Response(const Response& ref);

	Response&	operator=(const Response& response);

	void										addDefaultHeaders(void);
	void										setBody(ARequestType* requestResult, int socketFd);
	uint16_t									setErrorPage(uint16_t code, const ServerConfiguration& serverConfiguration);
	void										initValues(int code, const ServerConfiguration& serverConfiguration, ARequestType *requestResult, int socketFd);

public:
	Response(const ServerConfiguration &defaultConfig);
	~Response(void);

	void										setResponse(int code);
	void										setResponse(ARequestType& ARequestType, int socketFd);
	void										reset();

	const Status*								getStatus(void) const;
	const std::string*							getHeader(const std::string &key) const;
	const std::map<std::string, std::string>&	getHeaderMap(void) const;
	bool										getIsBlocking(void) const;
	SharedResource<int>							getSrcBodyFd(void) const;
	SharedResource<ABody*>						getBody(void) const;
	const std::string&							getAutoIndexPage(void) const;
};

std::ostream & operator<<(std::ostream & o, Response const & rhs);

#endif // !RESPONSE_HPP
