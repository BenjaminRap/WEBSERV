#ifndef SOCKETS_HANDLER
# define SOCKETS_HANDLER

# include <list>
# include <sys/epoll.h>

# include "SocketData.hpp"

class SocketsHandler
{
private:
	std::list<SocketData>	_socketsData;
	int						_epfd;
	epoll_event				*_events;
	int						_maxEvents;
	size_t					_eventsCount;

	void					closeSocket(const SocketData &socketData);
public:
	SocketsHandler(int maxEvents);
	~SocketsHandler();

	int		epollWaitForEvent();
	int		addFdToListeners(int fd, void (&callback)(int fd, void *data), void *data, uint32_t events);
	void	callSocketCallback(size_t eventIndex);
	bool	closeIfConnectionStopped(size_t eventIndex);
};

#endif // !SOCKETS_HANDLER