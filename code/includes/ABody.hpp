#ifndef A_BODY_HPP
# define A_BODY_HPP

# include <sys/types.h>	// for ssize_t

/**
 * @class ABody
 * @brief A purely abtrasct class representing a Body.
 * A body is the content of a request or response.
 * This class can write the content of that body into
 * another fd.
 * Depending on the Body type, the writing process
 * can be different (from a chunked body to a sized body).
 *
 */
class ABody
{
private:
	/**
	 * @brief The fd in which the body will write. It could be the
	 * client socket or a file fd ...
	 * If this value is -1, then the body will not write but will
	 * check for the body end.
	 */
	int			_fd;
	/**
	 * @brief True if this body has been entirely written.
	 */
	bool		_finished;

	ABody(const ABody& ref);
	
	ABody&	operator=(const ABody& ref);
	
protected:
	/**
	 * @brief Tag this instance has finished.
	 * @note It isn't reversible !
	 */
	void	setFinished();

	/**
	 * @brief Write the buffer content to the _fd.
	 *
	 * @param buffer The content of the body (or a part of it).
	 * @param bufferCapacity The size of the buffer. The string inside
	 * the buffer should take all the bufferCapacity because the body
	 * will parse all of it.
	 * @throw It should not throw.
	 * @return The number of character written (or ignored if the _fd is set to -1)
	 */
	virtual ssize_t		writeToFd(const void *buffer, size_t bufferCapacity) = 0;
public:
	ABody(int fd);
	virtual ~ABody();
	
	int			getFd() const;
	bool		getFinished() const;
	
	/**
	 * @brief Call the ABody child class writeToFd
	 *
	 * @param body The instance whose writeToFd method will called.
	 * @throw It should not throw
	 * @ref writeToFd The rest of the parameters and return value corresponds.
	 */
	static ssize_t	callInstanceWriteToFd(ABody &body, const void *buffer, size_t bufferCapacity);
};

#endif // !A_BODY_HPP
