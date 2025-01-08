#ifndef SOCKETS_HANDLER
# define SOCKETS_HANDLER

# include <list>
# include <sys/epoll.h>

# include "SocketData.hpp"

class SocketsHandler
{
private:
	static bool				_instanciated;

	std::list<SocketData>	_socketsData;
	int						_epfd;
	epoll_event				*_events;
	unsigned int			_maxEvents;
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