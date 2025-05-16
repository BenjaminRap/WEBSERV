#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include <vector>

# include "FlowBuffer.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConfiguration.hpp"

# define REQUEST_BUFFER_SIZE 1024

# define MAX_AGE_COOKIE_SEC "86400" // 86400 seconds = 24h

/**
 * @enum RequestState
 * @brief As the request can be sent in multiples blocks,
 * the RequestHandler has this enum to keep track of where
 * it was after the preceding read :
 * 		- REQUEST_STATUS_LINE
 * 		- REQUEST_HEADERS
 * 		- REQUEST_EMPTY_LINE
 * 		- REQUEST_BODY
 * 		- REQUEST_DONE
 *
 */
enum	RequestState
{
	REQUEST_STATUS_LINE,
	REQUEST_HEADERS,
	REQUEST_EMPTY_LINE,
	REQUEST_BODY,
	REQUEST_DONE,
	CONNECTION_CLOSED
};

class ARequestType;

/**
 * @class RequestHandler
 * @brief The class managing the request of the client :
 * it should read the request, divide it into the status line, the headers
 * the empty line and the body. Send these informations to the corresponding
 * classes.
 * Once the request is builded, it calls the GET, POST, PUT, DELETE classes.
 * With the result of that, it creates the response.
 *
 */
class RequestHandler
{
private:
	/**
	 * @brief The buffer in which the request will be written, if the
	 * status line and the headers doesn't fit in it, we send a HTTP_BAD_REQUEST..
	 */
	char									_buffer[REQUEST_BUFFER_SIZE];
	/**
	 * @brief The FlowBuffer managing the read of the request into the _buffer.
	 */
	FlowBuffer								_flowBuf;
	/**
	 * @brief It keeps track of where the request is. (reading the status line, a header ...)
	 */
	RequestState							_state;
	/**
	 * @brief The request that is builded with the client request.
	 */
	Request									_request;
	/**
	 * @brief A vector of all the ServerConfigurations of the host.
	 * The correct server configurations will be determined with the request
	 * host header.
	 */
	const std::vector<ServerConfiguration>	&_serverConfs;

	RequestHandler(const RequestHandler& ref);

	RequestHandler&	operator=(const RequestHandler& ref);

	/**
	 * @brief Read the status line.
	 * If the _state isn't REQUEST_STATUS_LINE, it does nothing.
	 * It skips the firsts empty lines.
	 * It then parse the status line.
	 * If there is an error, this function set the response
	 * status and change the _state to REQUEST_DONE.
	 * Otherwise, it set _state to REQUEST_HEADERS
	 */
	void						readStatusLine(Response &response);
	/**
	 * @brief Read one or multiples headers.
	 * If the _state isn't REQUEST_HEADERS, it does nothing.
	 * If there is an error, this function set the response
	 * status and change the _state to REQUEST_DONE.
	 * If there is an empty line, it set _state to REQUEST_EMPTY_LINE
	 */
	void						readHeaders(Response &response);
	/**
	 * @brief Execute the request that has been parsed with readStatusLine
	 * and readHeaders.
	 * If _state isn't REQUEST_EMPTY_LINE, it does nothing.
	 * It calls the right method, and set the response with the request
	 * results.
	 *
	 * @param socketFd the socket fd of the client.
	 */
	void						executeRequest(Response &response, EPollHandler& ePollHandler);
	/**
	 * @brief Write the request body from the _buffer to the body fd.
	 * If there is an error, it sets the response values.
	 *
	 */
	void						writeBodyFromBuffer(Response &response);
	/**
	 * @brief Returns the ServerConfiguration corresponding to the Host header.
	 * If no ServerConfiguration corresponds or if there is no Host header,
	 * it returns the first ServerConfiguration of the vector.
	 *
	 */
	const ServerConfiguration	&getServerConfiguration(const std::string& host) const;
	/**
	 * @brief Set  the response values depending on the results of the
	 * request.
	 *
	 */

	void						handleCookie(Headers &headers_response, const ServerConfiguration &serverConfiguration);

	void						processRequestResult
	(
		ARequestType& request,
		Response &response
	);
public:
	RequestHandler(const std::vector<ServerConfiguration> &serverConfs);
	~RequestHandler();

	/**
	 * @brief Redirect the body of the request from the _buffer to the response
	 * body fd, using the body writeToFd method.
	 *
	 * @return The new state of the request
	 */
	RequestState				redirectFirstPart(int socketFd, Response &response);
	/**
	 * @brief Redirect the body.
	 *
	 * @param socketFd The fd of the client.
	 * @param response The response that will be set if an error happend.
	 * @param canRead Can this function read from the socketFd.
	 * @return 
	 */
	RequestState				redirectBody(int socketFd, Response &response, bool canRead);
	/**
	 * @brief Read and handle the execution of the request
	 *
	 * @return The state of the request
	 */
	RequestState				readRequest(Response &response, int socketFd, EPollHandler& ePollHandler);
	/**
	 * @brief Returns if the _state is REQUEST_BODY
	 *
	 */
	bool						isStateRequestBody(void);
	/**
	 * @brief reset the response and set teh _state to READ_STATUS_LINE
	 *
	 */
	void						setNewRequest(void);
};

#endif // !REQUEST_HANDLER_HPP
