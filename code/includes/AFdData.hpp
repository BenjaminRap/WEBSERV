#ifndef A_FD_DATA_HPP
# define A_FD_DATA_HPP

# include <stdint.h>	// for uint32_t
# include <sys/types.h>	// for ssize_t

class EPollHandler;


/**
 * @brief A class wrapping a fd.
 * This class is abstract, so it can only be used through its childs.
 */
class AFdData
{
public:
	/**
	 * @brief The child class :
	 * 	- FILE_FD : a file
	 * 	- CGI_OUT : the cgi ouput
	 * 	- CGI_IN : the cgi input
	 * 	- SERVER_SOCKET_DATA : a server
	 * 	- CONNECTED_SOCKET_DATA : a connection client/server
	 */
	enum	AFdDataChilds
	{
		FILE_FD,
		CGI_OUT,
		CGI_IN,
		SERVER_SOCKET_DATA,
		CONNECTED_SOCKET_DATA
	};
	virtual ~AFdData(void);


	/**
	 * @brief This function is called when the _fd receives and events, the
	 * implementation depends on the child : a ServerSocketData will create
	 * a new connection, the ConnectedSocketData will manage a request ...
	 *
	 * @param events The events on which epoll will notify us.
	 */
	virtual void	callback(uint32_t events) = 0;

	/***************************** Getters ******************************/

	int				getFd(void) const;
	bool			getIsBlocking(void) const;
	bool			getIsActive(void) const;
	AFdDataChilds	getType(void) const;
protected:

	/**
	 * @brief set the fd and add the FD_CLOEXEC flag to it.
	 * @throw :
	 * 	- a std::logic_error if fd is already set
	 * 	- a std::invalid_argument if the newFd is negative.
	 * 	- a std::runtime_error if we can't add the flag.
	 * 	If itt throws, _fd isn't changed.
	 *
	 */
	void			setFd(int newFd);
	/**
	 * @brief Set the _isActive variable to false.
	 */
	void			setFinished(void);

	/**
	 * @throw Same as setFd.
	 */
	AFdData(int fd, AFdDataChilds type, bool isBlocking);
	AFdData(AFdDataChilds type);
private:
	/**
	 * @brief The file descriptor on a file, socket, pipe ...
	 */
	int				_fd;
	/**
	 * @brief A boolean indicating if this fd can still receive/send
	 * data. For example, if the other end of the fd has been closed,
	 * this variable is set to false.
	 */
	bool			_isActive;
	/**
	 * @brief Is this fd blocking (true for a pipe or a socket, false
	 * for a file).
	 * Even if the O_NONBLOCK flags has been added to the fd, it does not
	 * changes this variable.
	 */
	bool			_isBlocking;
	/**
	 * @brief The child class.
	 */
	AFdDataChilds	_type;

	AFdData(void);
	AFdData(const AFdData &ref);

	AFdData&	operator=(const AFdData& ref);
};

#endif // !A_FD_DATA_HPP
