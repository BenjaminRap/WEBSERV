#ifndef A_FD_DATA_HPP
# define A_FD_DATA_HPP

# include <stdint.h>	// for uint32_t

class SocketsHandler;

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
	const int	_fd;
	/**
	 * @brief Does this fd can block reading or writing ?
	 */
	const bool	_isBlocking;

	AFdData(int fd, bool isBlocking);
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
