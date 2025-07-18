#ifndef CGI_OUT_HPP
# define CGI_OUT_HPP

# include <cstdio>					// for L_tmpnam

# include "AEPollFd.hpp"
# include "FileFd.hpp"
# include "Headers.hpp"				// for Headers
# include "ServerConfiguration.hpp"	// for ServerConfiguration

# define CGI_OUT_EVENTS (EPOLLIN | EPOLLERR | EPOLLRDHUP | EPOLLHUP)

class	FlowBuffer;
class	Status;
class	CgiOutArgs;
class	ConnectedSocketData;

class CgiOut : public AEPollFd
{
private:
	enum	CgiOutState
	{
		READ_HEADER,
		CGI_TO_TEMP,
		WRITE_FIRST_PART,
		FILE_TO_BUFFER,
		CGI_TO_BUFFER,
		DONE
	};

	/**
	 * @brief The FlowBuffer of the RawResponse, what is written to
	 * it will be written to the client.
	 */
	FlowBuffer&						_flowBuf;
	/**
	 * @brief The first part of the response.
	 */
	std::string						_firstPart;
	/**
	 * @brief The number of character from the firstPart that
	 * has been written to the flowBuf
	 */
	size_t							_charsWritten;
	/**
	 * @brief The headers parsed from the cgi.
	 */
	Headers							_headers;
	/**
	 * @brief The name of the temporary file, or "\0" if no
	 * temporary file has been created.
	 */
	char							_tempName[L_tmpnam];
	/**
	 * @brief The source file, it can be the temporary file or the
	 * error page.
	 */
	FileFd*							_srcFile;
	CgiOutState						_state;
	/**
	 * @brief The code of the response that will be send to the client.
	 */
	uint16_t						_code;
	/**
	 * @brief A boolean indicating if there was an error parsing the response.
	 */
	bool							_error;
	const ServerConfiguration&		_serverConf;
	/**
	 * @brief Can the CgiOut write to the buffer. if the callback is called, this
	 * variable is set to true.
	 */
	bool							_canWrite;
	/**
	 * @brief A boolean indicating if we can Receive nwe EPOLLIN. If we can't
	 * that means that we won't get EAGAIN after a read.
	 */
	bool							_cgiReadFinished;
	/**
	 * @brief The pid of the cgi, it will be used in the destructor to kill the program.
	 */
	pid_t							_pid;
	/**
	 * @brief The headers that will be added to the cgi response headers.
	 */
	const std::list<ConfigHeaders>	_addHeader;
	ConnectedSocketData&			_connectedSocketData;

	CgiOut(void);
	CgiOut(const CgiOut &ref);

	CgiOut&			operator=(const CgiOut &ref);

	/**
	 * @brief Verify that the mandatory headers are there. It also sets
	 * the temporary file if content-length or transfer-encoding: chunked
	 * are not found. It the latter case, it sets the _state to CGI_TO_TEMP.
	 *
	 * @return HTTP_OK if the headers are correct, otherwise the HTTP error status.
	 */
	uint16_t	checkHeaders(void);
	/**
	 * @brief Get the status code from the Status header.
	 *
	 * @return The status of the response. It can be the status send by the cgi,
	 * or a http status error if the cgi result is incorrect. In the latter case,
	 * _error is set to true.
	 */
	uint16_t	getStatusCode(void);
	/**
	 * @brief Read all the headers available in the buffer.
	 * It checks the headers, get the status code from them and generate
	 * the first part if the headers are done.
	 */
	void		readHeaders(void);
	/**
	 * @brief generate the first part of the response. It checks the status,
	 * get the error page, and set the state to WRITE_FIRST_PART.
	 */
	void		generateFirstPart(void);
	/**
	 * @brief This method should be called if an error occured.
	 * It clears the headers, add the defaults one, and set the custom
	 * error page.
	 *
	 * @param currentStatus The current status, it may be changed if an error
	 * occured while opening the error pages.
	 * It uses the same function as the Response to get the error page.
	 */
	void		setErrorPage(const Status** currentStatus);
	/**
	 * @brief This method should be called when we can't read
	 * from the cgi anymore because of an error. Depending on the state of this
	 * instance, the behaviour changes.
	 */
	void		handleCgiError(uint16_t code);
	/**
	 * @brief Read from the cgi and write it into the _flowBuf.
	 * This method should only be called if a EPOLLIN  events has
	 * been registered.
	 */
	void		readFromCgi(void);
	/**
	 * @brief Write the cgi body into a temporary file.
	 * It is usefull if the cgi doesn't set the content-length
	 * or transfer-encoding: chunked headers.
	 */
	void		writeToTemp(void);
	/**
	 * @brief Write the firt part (status line, headers, empty line)
	 * into the buffer. It uses the same function as the RawResponse
	 * to generate the first part.
	 */
	void		writeFirstPart(void);
	/**
	 * @brief Write the body from the cgi or the temporary file
	 * and write it to the _flowBuf.
	 */
	void		writeToBuff(void);
	/**
	 * @brief Set the _state to DONE and _isActive to false.
	 * The fd shouldn't be read from after this method is called.
	 */
	void		setFinished(void);

public:
	CgiOut
	(
		int fd,
		EPollHandler& ePollHandler,
		pid_t pid,
		const CgiOutArgs& cgiOutArgs,
		ConnectedSocketData& connectedSocketData
	);
	~CgiOut();

	void		callback(uint32_t events);
	bool		isResponseReady(void) const;
	void		checkTime(time_t now);
};

#endif // !CGI_OUT_HPP
