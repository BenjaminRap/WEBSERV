#ifndef A_FD_DATA_HPP
# define A_FD_DATA_HPP

# include "ServerConfiguration.hpp"
# include <stdint.h>
# include <list>

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
	int										_fd;
	/**
	 * @brief The iterator of this instance in the SocketsHandler list. It is used
	 * to remove this instance from the list in O(1).
	 */
	std::list<AFdData *>::iterator			_iterator;
	/**
	 * @brief True if the setIterator has been called with a good argument.
	 */
	bool									_isIteratorSet;
	/**
	 * @brief The class managing all the sockets.
	 */
	SocketsHandler							&_socketsHandler;
	const std::vector<ServerConfiguration>	&_serverConfigurations;

	AFdData(int fd, SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> &serverConfigurations);
private:
	AFdData(void);
	AFdData(const AFdData &ref);

	AFdData&	operator=(const AFdData& ref);
public:
	virtual ~AFdData(void);

	virtual void							callback(uint32_t events) = 0;

	int										getFd() const;
	const std::list<AFdData *>::iterator		&getIterator() const;
	void									setIterator(const std::list<AFdData *>::iterator &iterator);
};

#endif // !A_FD_DATA_HPP
