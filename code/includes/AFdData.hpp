#ifndef A_FD_DATA_HPP
# define A_FD_DATA_HPP

# include <stdint.h>	// for uint32_t

class EPollHandler;

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

	AFdData(int fd, EPollHandler& ePollHandler);
	AFdData(int fd);
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
};

#endif // !A_FD_DATA_HPP
