#ifndef A_FD_DATA_HPP
# define A_FD_DATA_HPP

# include <stdint.h>	// for uint32_t
# include <sys/types.h>	// for ssize_t

class EPollHandler;


/**
 * @brief Every FDs that are in the listeners of epoll has a corresponding FdData
 * instance. This class store everything that the programs needs when the fd
 * receive an events (EPOLLIN/EPOLLOUT/...).
 * This class is abstract, so it can only be used through its childs.
 */
class AFdData
{
public:
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
	 * @brief The file descriptor on a file, socket, pipe ...
	 */
	int				_fd;
	/**
	 * @brief A boolean indicating if this fd can still receive/send
	 * data. For example, if the other end of the fd has been closed,
	 * this variable is set to false.
	 */
	bool			_isActive;
	bool			_isBlocking;
	AFdDataChilds	_type;
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
protected:

	void			setFd(int fd);
	AFdData(int fd, AFdDataChilds type, bool isBlocking);
	AFdData(AFdDataChilds type);
private:
	AFdData(void);
	AFdData(const AFdData &ref);

	AFdData&	operator=(const AFdData& ref);
};

#endif // !A_FD_DATA_HPP
