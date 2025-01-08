#ifndef SOCKETS_HANDLER
# define SOCKETS_HANDLER

# include <list>
# include <sys/epoll.h>

# include "SocketData.hpp"

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
	static bool				_instanciated;

	/**
	 * @brief A list of the SocketData that are in the epoll interest list.
	 */
	std::list<SocketData>	_socketsData;
	/**
	 * @brief The epoll fd.
	 */
	int						_epfd;
	/**
	 * @brief The events array that will be passed at epoll_wait. It stores all
	 * the events that the epoll_wait function has detected.
	 */
	epoll_event				*_events;
	/**
	 * @brief The length of the _events array.
	 */
	unsigned int			_maxEvents;
	/**
	 * @brief The count of events detected by the epoll_wait function.
	 */
	size_t					_eventsCount;

	SocketsHandler(const SocketsHandler& ref);
	SocketsHandler(void);

	SocketsHandler&	operator=(const SocketsHandler &ref);

	void			closeSocket(int fd);
public:
	SocketsHandler(unsigned int maxEvents);
	~SocketsHandler();

	int				epollWaitForEvent();
	int				addFdToListeners(int fd, void (&callback)(int fd, void *data), void *data, uint32_t events);
	void			callSocketCallback(size_t eventIndex);
	bool			closeIfConnectionStopped(size_t eventIndex);
};

#endif // !SOCKETS_HANDLER