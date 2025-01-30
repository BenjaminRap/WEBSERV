#ifndef SOCKETS_HANDLER_HPP
# define SOCKETS_HANDLER_HPP

# include <list>
# include <sys/epoll.h>
# include <vector>
# include <string>
# include <iostream>

# include "SocketData.hpp"
#include <socketCommunication.hpp>

class Configuration;
class Host;

/**
 * @brief Manage the epoll functions : add or remove fd to the interest list and
 * execute the socket callback on events.
 */
class SocketsHandler
{
private:
	/**
	 * @brief True if there is an instance of this class.
	 */
	static bool					_instanciated;

	/**
	 * @brief A list of the SocketData that are in the epoll interest list.
	 */
	std::list<SocketData*>		_socketsData;
	/**
	 * @brief The epoll fd.
	 */
	int							_epfd;
	/**
	 * @brief The events array that will be passed at epoll_wait. It stores all
	 * the events that the epoll_wait function has detected.
	 */
	epoll_event					*_events;
	/**
	 * @brief The length of the _events array.
	 */
	unsigned int				_maxEvents;
	/**
	 * @brief The count of events detected by the epoll_wait function.
	 */
	size_t						_eventsCount;
	/**
	 * @brief When creating a socket unix connection, a socket unix file is
	 * created in the file system. This vector stores all the sockets path that
	 * have been created.
	 * It should be used to removed them in the destructor.
	 */
	std::vector<std::string>	_unixSocketsToRemove;

	SocketsHandler(const SocketsHandler& ref);
	SocketsHandler(void);

	SocketsHandler&	operator=(const SocketsHandler &ref);

	void			closeSocket(int fd);
public:
	SocketsHandler(const Configuration &conf);
	~SocketsHandler();

	int		epollWaitForEvent();
	int		addFdToListeners(SocketData &socketData, uint32_t events);
	void	callSocketCallback(size_t eventIndex) const;
	bool	closeIfConnectionStopped(size_t eventIndex);
	int		bindFdToHost(int fd, const Host &host);
};

#endif // !SOCKETS_HANDLER_HPP