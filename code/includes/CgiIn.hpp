#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "AEPollFd.hpp"
# include "CgiOutArgs.hpp"
# include "FileFd.hpp"
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
	/**
	 * @brief The name of the temporary file, if the body is chunked.
	 * It will be used to know the full size of the body.
	 * If the body is size, the first character is '\0'.
	 */
	char						_tempName[L_tmpnam];
	/**
	 * @brief A FileFd opened on the _tempName file.
	 */
	FileFd*						_tmpFile;
	/**
	 * @brief The argv that will be passed to execCGI, if the body
	 * is chunked, otherwise, it is empty.
	 */
	const char*					_argv[3];
	/**
	 * @brief The env that will be passed to execCGI, if the body
	 * is chunked, otherwise, it is empty.
	 */
	const char*					_env[23];
	/**
	 * @brief A class passed to the CgiOut constructor, if the body
	 * is chunked, otherwise, it is NULL.
	 */
	const CgiOutArgs * const	_cgiOutArgs;

	CgiIn(void);
	CgiIn(const CgiIn &ref);

	CgiIn&	operator=(const CgiIn &ref);

	/**
	 * @brief Set the response status, set _isActive to false
	 * and call the ConnectedSocketData readNextRequests method.
	 *
	 * @param code The code passed to the setResponse method.
	 * if it is not set to 0, the response code will be set  to code.
	 * @throw It can throw a std::bad_alloc, due to the readNextRequests method.
	 */
	void		setFinished(uint16_t code);
	/**
	 * @brief get the temporary file size, add it to the env, execute the cgi fork,
	 * add the CgiOut instance to epoll and set the _state to CgiIn::TEMP_TO_CGI
	 */
	void		execCgi(void);
public:
	/**
	 * @throw Throw a std::runtime_error if we can't create the temporary file.
	 *
	 */
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
