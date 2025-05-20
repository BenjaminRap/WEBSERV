#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "AFdData.hpp"	// for AFdData
# include "FileFd.hpp"	// for FileFd
# include <cstdio>		// for L_tmpnam

# define CGI_IN_EVENTS (EPOLLOUT | EPOLLERR | EPOLLHUP)

class	ConnectedSocketData;
class	Response;

class	FlowBuffer;
class	ABody;
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
	 * @brief The method called when a events occured.
	 * It manages the differents events and redirect the body, either
	 * to the temporary file, or the cgi.
	 */
	void	callback(uint32_t events);
};

#endif // !CGI_IN_HPP
