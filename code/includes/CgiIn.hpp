#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "AFdData.hpp"	// for AFdData
# include "FileFd.hpp"	// for FileFd
# include <cstdio>		// for L_tmpnam

# define CGI_IN_EVENTS (EPOLLOUT | EPOLLERR | EPOLLRDHUP | EPOLLHUP)

class	ConnectedSocketData;
class	Response;

class	FlowBuffer;
class	ABody;

enum	CgiInState
{
	BUF_TO_TEMP,
	TEMP_TO_CGI,
	BUF_TO_CGI
};

class CgiIn : public AFdData
{
private:
	/**
	 * @brief The FlowBuffer of the request,
	 * the body of the client is written in it.
	 */
	FlowBuffer&				_flowBuf;
	/**
	 * @brief The body that will manages the writing
	 * of the client body.
	 */
	ABody&					_body;
	/**
	 * @brief The socket managing the client requests.
	 * It is needed because this class has to call a method
	 * when the body redirection is done.
	 */
	ConnectedSocketData&	_connectedSocketData;
	/**
	 * @brief The response that will be sent to the client.
	 */
	Response&				_response;
	/**
	 * @brief The name of the temporary file needed to unchunk the request.
	 * If it isn't needed, it is set to '\0'.
	 */
	char					_tempName[L_tmpnam];
	/**
	 * @brief The temporary file, or NULL if there is not.
	 */
	FileFd*					_tempFile;
	CgiInState				_state;

	CgiIn(void);
	CgiIn(const CgiIn &ref);

	CgiIn&	operator=(const CgiIn &ref);

public:
	CgiIn
	(
		int fd,
		EPollHandler& ePollHandler,
		FlowBuffer& requestFlowBuffer,
		ABody&		body,
		ConnectedSocketData& connectedSocketData,
		Response& currentResponse
	);
	~CgiIn();

	/**
	 * @brief Set the response status, set _isActive to false
	 * and call the ConnectedSocketData readNextRequests method.
	 *
	 * @param code The code passed to the setResponse method.
	 */
	void	setFinished(uint16_t code);
	/**
	 * @brief Redirect the client body into the temporary file,
	 * using the _body to unchunk. This method is called only if
	 * the _body is a ChunkedRequest.
	 */
	void	redirectToTemp(void);
	/**
	 * @brief The method called when a events occured.
	 * It manages the differents events and redirect the body, either
	 * to the temporary file, or the cgi.
	 */
	void	callback(uint32_t events);
};

#endif // !CGI_IN_HPP
