#ifndef A_FD_DATA_HPP
# define A_FD_DATA_HPP

# include <stdint.h>	// for uint32_t

class EPollHandler;

enum	AFdDataChilds
{
	FILEFD,
	CGIOUT,
	CGIIN,
	SERVER_SOCKET_DATA,
	CONNECTED_SOCKET_DATA
};

/**
 * @brief Every FDs that are in the listeners of epoll has a corresponding FdData
 * instance. This class store everything that the programs needs when the fd
 * receive an events (EPOLLIN/EPOLLOUT/...).
 * This class is abstract, so it can only be used through its childs.
 */
class AFdData
{
protected:
	/**
	 * @brief The file descriptor on a file, socket, pipe ...
	 */
	const int		_fd;
	/**
	 * @brief The class managing all fds in epoll, including this one,
	 * if it is blocking.
	 * If the fd is non blocking, this variable is set to NULL.
	 */
	EPollHandler*	_ePollHandler;
	/**
	 * @brief A boolean indicating if this fd can still receive/send
	 * data. For example, if the other end of the fd has been closed,
	 * this variable is set to false.
	 */
	bool			_isActive;
	AFdDataChilds	_type;

	AFdData(int fd, EPollHandler& ePollHandler, AFdDataChilds type);
	AFdData(int fd, AFdDataChilds type);
private:
	AFdData(void);
	AFdData(const AFdData &ref);

	AFdData&	operator=(const AFdData& ref);
public:
	virtual ~AFdData(void);

	/**
	 * @brief This function is called when the _fd receives and events, the
	 * implementation depends on the child : a ServerSocketData will create
	 * a new connection, the ConnectedSocketData will manage a request ...
	 *
	 * @param events 
	 */
	virtual void	callback(uint32_t events) = 0;

	int				getFd(void) const;
	bool			getIsBlocking(void) const;
	bool			getIsActive(void) const;
	AFdDataChilds	getType(void) const;
};

#endif // !A_FD_DATA_HPP
