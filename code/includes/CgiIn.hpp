#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "ASocketData.hpp"
# include "CgiOutArgs.hpp"
# include "FileFd.hpp"
# include "FlowBuffer.hpp"
# include <cstdio>		// for L_tmpnam

class	SizedBody;
class	ChunkedBody;
class	ABody;


# define CGI_IN_EVENTS (EPOLLOUT | EPOLLERR | EPOLLHUP)

class	ConnectedSocketData;
class	Response;
class	CgiOut;
class	FlowBuffer;
class	ABody;

class CgiIn : public AEPollFd
{
private:
	enum	CgiInState
	{
		BUFFER_TO_TEMP,
		BUFFER_TO_CGI,
		TEMP_TO_CGI,
		DONE
	};

	/**
	 * @brief The FlowBuffer of the request,
	 * the body of the client is written in it.
	 */
	FlowBuffer&					_flowBuf;
	/**
	 * @brief The body that will manages the writing
	 * of the client body.
	 */
	ABody&						_body;
	/**
	 * @brief The socket managing the client requests.
	 * It is needed because this class has to call a method
	 * when the body redirection is done.
	 */
	ConnectedSocketData&		_connectedSocketData;
	/**
	 * @brief The response that will be sent to the client.
	 */
	Response&					_response;
	CgiInState					_state;
	char						_tempName[L_tmpnam];
	FileFd*						_tmpFile;
	const char*					_argv[3];
	const char*					_env[23];
	const CgiOutArgs * const	_cgiOutArgs;

	CgiIn(void);
	CgiIn(const CgiIn &ref);

	CgiIn&	operator=(const CgiIn &ref);

	/**
	 * @brief Set the response status, set _isActive to false
	 * and call the ConnectedSocketData readNextRequests method.
	 *
	 * @param code The code passed to the setResponse method.
	 */
	void		setFinished(uint16_t code);
	void		execCgi(void);
	FlowState	writeBody(uint32_t events);
public:
	CgiIn
	(
		EPollHandler& ePollHandler,
		FlowBuffer& requestFlowBuffer,
		ChunkedBody& chunkedBody,
		ConnectedSocketData& connectedSocketData,
		Response& currentResponse,
		const char* argv[3],
		const char* env[23],
		const CgiOutArgs * cgiOutArgs
	);
	CgiIn
	(
		int fd,
		EPollHandler& ePollHandler,
		FlowBuffer& requestFlowBuffer,
		SizedBody& sizedBody,
		ConnectedSocketData& connectedSocketData,
		Response& currentResponse
	);
	~CgiIn();

	/**
	 * @brief The method called when a events occured.
	 * It manages the differents events and redirect the body, either
	 * to the temporary file, or the cgi.
	 */
	void		callback(uint32_t events);
};

#endif // !CGI_IN_HPP
