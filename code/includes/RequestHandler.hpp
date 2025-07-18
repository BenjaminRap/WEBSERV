#ifndef REQUEST_HANDLER_HPP
# define REQUEST_HANDLER_HPP

# include <vector>

# include "FlowBuffer.hpp"
# include "Request.hpp"
# include "Response.hpp"
# include "ServerConfiguration.hpp"

# define REQUEST_BUFFER_SIZE 1024

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
	REQUEST_DONE
};

class	RequestContext;
class	ARequestType;

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
	FlowBuffer								_requestBuf;
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
	void						executeRequest(Response &response, RequestContext& requestContext);
	/**
	 * @brief Returns the ServerConfiguration corresponding to the Host header.
	 * If no ServerConfiguration corresponds or if there is no Host header,
	 * it returns the first ServerConfiguration of the vector.
	 *
	 */
	const ServerConfiguration	&getServerConfiguration(std::string host) const;
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
	 * @param socketFd A pointer on the client/server communication fd, or NULL, if we
	 * can't read from it.
	 * @param response The currentResponse, its code will be set if an error occured.
	 * @return 
	 */
	RequestState				redirectBody(const int* socketFd, Response &response);
	/**
	 * @brief Read and handle the execution of the request
	 *
	 * @return The state of the request
	 */
	RequestState				readRequest(RequestContext& requestContext, Response& response);
	/**
	 * @brief Returns if the _state is REQUEST_BODY
	 *
	 */
	bool						isStateRequestBody(void);
	/**
	 * @brief reset the response and set the _state to READ_STATUS_LINE
	 *
	 */
	void						setNewRequest(void);
	/**
	 * @brief Remove the request fdData and redirect the body.
	 *
	 */
	RequestState				ignoreBody(Response& response);
	Request&					getRequest(void);
	FlowBuffer&					getFlowBuffer(void);
};

#endif // !REQUEST_HANDLER_HPP
