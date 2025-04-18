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
	const int								_fd;
	/**
	 * @brief The iterator of this instance in the SocketsHandler list. It is used
	 * to remove this instance from the list in O(1).
	 */
	std::list<AFdData *>::iterator			_iterator;
	/**
	 * @brief True if the setIterator has been called with a valid argument.
	 */
	bool									_isIteratorSet;
	/**
	 * @brief The class managing all the sockets, including this one.
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

	/**
	 * @brief This function is called when the _fd receives and events, the
	 * implementation depends on the child : a ServerSocketData will create
	 * a new connection, the ConnectedSocketData will manage a request ...
	 *
	 * @param events 
	 */
	virtual void							callback(uint32_t events) = 0;

	int										getFd() const;
	/**
	 * @brief Return the iterator pointing to this object in the SocketHandler _socketsData
	 * list.
	 * @throw If the iterator hasn't been set with the setIterator function, throw
	 * a std::logic_error.
	 * @return A const reference on the iterator pointing to this object.
	 */
	const std::list<AFdData *>::iterator&	getIterator() const;
	/**
	 * @brief Set the _iterator of this FdData to a copy of the iterator passed
	 * as argument.
	 * If the iterator has already been set, print an error.
	 * If the FdData pointed by the iterator isn't this class, print an error.
	 * @param iterator The iterator that points to this FdData.
	 */
	void									setIterator(const std::list<AFdData *>::iterator &iterator);
};

#endif // !A_FD_DATA_HPP
