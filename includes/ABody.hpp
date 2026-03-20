#ifndef A_BODY_HPP
# define A_BODY_HPP

# include <stdint.h>	// for uint16_t
# include <sys/types.h>	// for ssize_t


/**
 * @class ABody
 * @brief A purely abtrasct class representing a Body.
 * A body is the content of a request.
 * This class can write the content of that body into
 * another fd. (ex : A file or the cgi)
 * Depending on the Body type, the writing process
 * can be different (from a chunked body to a sized body).
 *
 */
class ABody
{
public:
	/**
	 * @brief The child class of the ABody:
	 * 	- SIZED : a body with a known size.
	 * 	- CHUNKED : a chunk body, whose size is unknown.
	 */
	enum	ABodyChilds
	{
		SIZED,
		CHUNKED
	};

	virtual ~ABody();

	/************************* Getters *********************************/

	bool		getFinished() const;
	uint16_t	getStatus() const;
	size_t		getWritten(void) const;
	ABodyChilds	getType(void) const;

	/*********************** Setters ***********************************/

	void		setFd(int fd, bool isFdBlocking);

	/*********************** Public Methods ***************************/


	/**
	 * @brief Call the ABody child class writeToFd
	 *
	 * @param body The instance whose writeToFd method will called.
	 * @throw It should not throw, unless the instancec's writeToFd throws.
	 * This behaviour shouldn't happend.
	 * @ref writeToFd The rest of the parameters and return value corresponds.
	 */
	static ssize_t	writeToFd(ABody &body, const void *buffer, size_t bufferCapacity);
private:
	/**
	 * @brief The fd in which the body will write. It could be the
	 * client socket or a file fd ...
	 * If this value is -1, then the body will not write but will
	 * check for the body end.
	 */
	int			_fd;
	/**
	 * @brief A boolean indicating if the fd is blocking (a pipe or socket).
	 * If it is set to true, the body can only write once in a writeToFd.
	 */
	bool		_isFdBlocking;
	/**
	 * @brief True if this body has been entirely written.
	 */
	bool		_finished;
	/**
	 * @brief The error status if an error happened, or HTTP_OK
	 * if there was no error. When the writeToFd function returns
	 * a -1, this value should not be set to HTTP_OK.
	 */
	uint16_t	_status;
	/**
	 * @brief The number of character written in the fd.
	 * if the fd is invalid, this attribute is always 0.
	 */
	size_t		_written;
	/**
	 * @brief An enum representing the child class.
	 */
	ABodyChilds	_type;
	/**
	 * @brief A boolean indicating if this ABody has written in
	 * fd. This variable is set to false at the start of the writeToFd
	 * method, and set to true when writeOrIgnore is called if the fd is positive
	 * and _isFdBlocking is set to true.
	 */
	bool		_hasWritten;

	ABody(const ABody& ref);
	
	ABody&	operator=(const ABody& ref);
	
protected:
	ABody(int fd, ABodyChilds type, bool isFdBlocking);
	/**
	 * @brief @note This constructor set teh fd to -1.
	 *
	 * @param type 
	 */
	ABody(ABodyChilds type);
	/**
	 * @brief Tag this instance has finished.
	 * It sets _finished to true and set the _status.
	 * @note It isn't reversible !
	 */
	void	setFinished(uint16_t status);

	bool	getHasWritten(void) const;

	/**
	 * @brief Write bufferCapacity bytes from the buffer to the _fd.
	 * If the _fd is negative, it justs ignores them.
	 * If the fd is blocking, this function can only be called once in
	 * a writeToFd.
	 *
	 * @param buffer The buffer whose characters will be written.
	 * @param bufferCapacity The number of characters to write.
	 * @return The number of characters written, or, if _fd is negative : bufferCapacity.
	 * If write fails, it reutns -1;
	 * @note It add the number of characters written in _written private attribute.
	 * @ref getWritten
	 */
	ssize_t		writeOrIgnore(const void* buffer, size_t bufferCapacity);
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
};

#endif // !A_BODY_HPP
