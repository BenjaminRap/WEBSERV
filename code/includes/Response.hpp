#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <stdint.h>  				// for uint16_t
# include <iostream>  				// for ostream
# include <string>    				// for string, basic_string

# include "ABody.hpp"				// for ABody
# include "AFdData.hpp"				// for AFdData
# include "Headers.hpp"				// for Headers
# include "ServerConfiguration.hpp"	// for ServerConfiguration
# include "SharedResource.hpp"		// for SharedResource

class Status;

class	ARequestType;

/**
 * @brief The classe that stores all the data that will be sent to the client.
 * This class should be converted into a RawResponse to be used.
 * The creation of this class is not performant but displays the response variable
 * more clearly.
 * The use of this class isn't mandatory because it has few uses and costs performances.
 */
class Response
{
private:
	/**
	 * @brief The status of the response. If setResponse hasn't been
	 * called yet, it is set at NULL.
	 */
	const Status*				_status;
	Headers						_headers;
	/**
	 * @brief A SharedResource on the body source fd.
	 * It isn't the fd stored in the _body, but the fd
	 * whose content will be written into the buffer.
	 * The body will then write the buffer into the client socket.
	 */
	SharedResource<AFdData*>	_fdData;
	/**
	 * @brief The body of the response, it could be a sized body, a
	 * chunked body ...
	 * @note It could also be NULL.
	 */
	SharedResource<ABody*>		_body;
	/**
	 * @brief The default ServerConfiguration, used when
	 * an error occured before the request execution.
	 */
	const ServerConfiguration&	_defaultConfig;
	/**
	 * @brief The string containing the auto index page.
	 * If there is no index page, this string is empty.
	 */
	std::string					_autoIndexPage;

	Response(void);
	Response(const Response& ref);

	Response&	operator=(const Response& response);

	/**
	 * @brief Set the body and the content-length header depending on
	 * the requestResult.
	 *
	 * @param requestResult The result of the request. If an error occured
	 * before the request execution, this variable should be set to NULL.
	 */
	void					setBody(int socketFd);
	/**
	 * @brief If there is an error, stop the managment of the body and replace
	 * it with the custom error page (if there is one).
	 *
	 * @return The new http status (itt could be the same as the previous one).
	 * It could be different if an error occured while opening the custom error
	 * page.
	 */
	const Status*			setErrorPage
	(
		const Status* status,
		const ServerConfiguration& serverConfiguration
	);
	/**
	 * @brief Initialise the values of the response : code, text, headers, body
	 *
	 * @param requestResult The result of the request. If an error occured
	 * before the request execution, this variable should be set to NULL.
	 */
	void					initValues
	(
		int code,
		const ServerConfiguration& serverConfiguration,
		int socketFd
	);

public:
	Response(const ServerConfiguration &defaultConfig);
	~Response(void);

	void							setResponse(int code);
	void							setResponse(ARequestType& ARequestType, int socketFd);
	/**
	 * @brief Reset this instance as it was after construction.
	 */
	void							reset();

	const Status*					getStatus(void) const;
	const Headers&					getHeaders(void) const;
	Headers&						getHeaders(void);
	SharedResource<AFdData*>		getFdData(void) const;
	SharedResource<ABody*>			getBody(void) const;
	const std::string&				getAutoIndexPage(void) const;
};

std::ostream & operator<<(std::ostream & o, Response const & rhs);

#endif // !RESPONSE_HPP
