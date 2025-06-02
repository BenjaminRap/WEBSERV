#ifndef CGI_IN_SIZED_HPP
# define CGI_IN_SIZED_HPP

# include "AEPollFd.hpp"
# include "CgiOutArgs.hpp"
# include "FlowBuffer.hpp"
# include <cstdio>		// for L_tmpnam

class	SizedBody;
class	ChunkedBody;
class	ABody;
class	ConnectedSocketData;
class	Response;
class	CgiOut;
class	FlowBuffer;

# define CGI_IN_EVENTS (EPOLLOUT | EPOLLERR | EPOLLHUP)


class CgiInSized : public AEPollFd
{
private:
	/**
	 * @brief The FlowBuffer of the request,
	 * the body of the client is written in it.
	 */
	FlowBuffer&				_requestBuf;
	/**
	 * @brief The body that will manages the writing
	 * of the client body.
	 */
	SizedBody&				_body;
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

	CgiInSized(void);
	CgiInSized(const CgiInSized &ref);

	CgiInSized&	operator=(const CgiInSized &ref);

	/**
	 * @brief Set the response status, set _isActive to false
	 * and call the ConnectedSocketData readNextRequests method.
	 *
	 * @param code The code passed to the setResponse method.
	 * if it is not set to 0, the response code will be set  to code.
	 * @throw It can throw a std::bad_alloc, due to the readNextRequests method.
	 */
	void		setFinished(uint16_t code);
public:
	CgiInSized
	(
		int fd,
		EPollHandler& ePollHandler,
		FlowBuffer& requestFlowBuffer,
		SizedBody& sizedBody,
		ConnectedSocketData& connectedSocketData,
		Response& currentResponse
	);
	~CgiInSized();

	/**
	 * @brief The method called when a events occured.
	 * It manages the differents events and redirect the body, either
	 * to the temporary file, or the cgi.
	 */
	void		callback(uint32_t events);
	void		checkTime(void);
};

#endif // !CGI_IN_SIZED_HPP
