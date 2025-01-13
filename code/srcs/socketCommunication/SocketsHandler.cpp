#include <unistd.h>
#include <sys/stat.h>
#include <cstdio>

#include "SocketsHandler.hpp"
#include "socketCommunication.hpp"

static int		removeSocketIfExists(const char sun_path[108]);
static int		bindUnixSocket(int fd, const sockaddr *addr, socklen_t addrLen, std::vector<std::string> &socketsToRemove);

bool	SocketsHandler::_instanciated = false;

/**
 * @brief Return the count of AF_UNIX family in the confiurations hosts.
 */
static size_t getUnixSocketCount(const Configuration &conf)
{
	size_t	unixAddrCount;

	unixAddrCount = 0;
	for (Configuration::const_iterator ci = conf.begin(); ci != conf.end(); ci++)
	{
		if ((*ci).first.getFamily() == AF_UNIX)
			unixAddrCount++;
	}
	return (unixAddrCount);
}

/**
 * @brief Create a SocketHandler, allocate the event array and create the epoll fd.
 * It also create a vector of sockets path to destroy, its size is the number of
 * unix socket hosts in the Configuration class.
 * This class can only has one instance.
 * @param conf The SocketsHandler use the configuration to initialize its variables.
 * @throw Throw an error if the allocation failed (std::bad_alloc), epoll_create
 * failed (std::exception) or this class already has an instance (std::logic_error).
 */
SocketsHandler::SocketsHandler(const Configuration &conf) :
	_maxEvents(conf.getMaxEvents()),
	_eventsCount(0),
	_socketsToRemove(getUnixSocketCount(conf))
{
	if (SocketsHandler::_instanciated == true)
		throw std::logic_error("Error : trying to instantiate a SocketsHandler multiple times");
	SocketsHandler::_instanciated = true;
	_events = new epoll_event[conf.getMaxEvents()]();
	_epfd = checkError(epoll_create(1), -1, "epoll_create() :");
	if (_epfd == -1)
	{
		delete [] _events;
		throw std::exception();
		return ;
	}
}

/**
 * @brief Free the events array, close the sockets and close the epoll fd.
 */
SocketsHandler::~SocketsHandler()
{
	SocketsHandler::_instanciated = false;
	for (std::list<SocketData>::const_iterator ci = _socketsData.begin(); ci != _socketsData.end(); ci++)
	{
		closeSocket((*ci).getFd());
	}
	checkError(close(_epfd), -1, "close() :");
	delete [] _events;
	for (std::vector<std::string>::iterator ci = _socketsToRemove.begin(); ci != _socketsToRemove.end(); ci++)
	{
		removeSocketIfExists((*ci).c_str());
	}
}

/**
 * @brief Close a socket and remove if from the epoll interest list. It does not
 * remove it from The _socketsData list.
 * @param fd The fd of the socket to close.
 */
void	SocketsHandler::closeSocket(int fd)
{
	checkError(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL), -1, "epoll_ctl() : ");
	checkError(close(fd), -1, "close() : ");
	std::cout << "Closing socket,fd : " << fd << std::endl;
}

/**
 * @brief Call epoll_wait with the SocketHandler variables and return its result;
 * @return The number of events, or -1 on error;
 */
int	SocketsHandler::epollWaitForEvent()
{
	const int	nfds = epoll_wait(_epfd, _events, _maxEvents, -1);

	if (nfds == -1)
		_eventsCount = 0;
	else
		_eventsCount = nfds;
	return (nfds);
}

/**
 * @brief Add the fd to the epoll listener, set the event.events and event.data.ptr
 * to a SocketData.
 * @param fd The fd of the socket to add to listeners, it will also be passed
 * to the callback function.
 * @param callback It is used as a parameter of the SocketData constructor, when
 * the epoll_wait see a read/write, we can get the SocketData callback from
 * the event.data.ptr.
 * @param data A data that will be passed to the callback.
 * @param events The events used to create the epoll_event variable
 * @return 0 on success, -1 on error and an error message written in the terminal.
 */
int	SocketsHandler::addFdToListeners(int fd, void (&callback)(int fd, void *data), void *data, uint32_t events)
{
	epoll_event	event;

	try
	{
		_socketsData.push_front(SocketData(fd, data, callback));
	}
	catch(const std::exception& e)
	{
		std::cerr << "push_front() : " << e.what() << std::endl;
		return (-1);
	}
	SocketData	&socketData = _socketsData.front();

	socketData.setIterator(_socketsData.begin());
	event.data.ptr = &(socketData);
	event.events = events;
	if (checkError(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event), -1, "epoll_ctl() :") == -1)
	{
		_socketsData.pop_front();
		return (-1);
	}
	return (0);
}

/**
 * @brief Call the callback of the socket, in the epoll events at eventIndex.
 * @param eventIndex The index of the event to check, [0, eventCount] where eventCount 
 * is the result of epoll_wait or epollWaitForEvent function.
 */
void	SocketsHandler::callSocketCallback(size_t eventIndex) const
{
	if (eventIndex >= _eventsCount)
	{
		std::cerr << "SocketsHandler callSocketCallback method was called with a wrong index" << std::endl;
		return ;
	}
	if (!(_events[eventIndex].events & (EPOLLIN | EPOLLOUT)))
		return ;
	const SocketData	&socketData = *(static_cast<SocketData *>(_events[eventIndex].data.ptr));

	socketData.callback();
}

/**
 * @brief If the socket at eventIndex has an EPOLLHUP or EPOLLRDHUP event, close it
 * and remove it from the _socketsData list.
 * @param eventIndex The index of the event to check, [0, eventCount] where eventCount 
 * is the result of epoll_wait or epollWaitForEvent function.
 * @return true if the connection is closed, false otherwise.
 */
bool	SocketsHandler::closeIfConnectionStopped(size_t eventIndex)
{

	if (eventIndex >= _eventsCount)
	{
		std::cerr << "SocketsHandler closeIfConnectionStopped method was called with a wrong index" << std::endl;
		return (false);		
	}
	if ((_events[eventIndex].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) == false)
		return (false);
	const SocketData	&socketData = *static_cast<SocketData *>(_events[eventIndex].data.ptr);
	const int	fd = socketData.getFd();

	closeSocket(fd);
	std::cout << "A connection stopped, fd : " << fd << std::endl;
	try
	{
		_socketsData.erase(socketData.getIterator());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (true);
}


/**
 * @brief Bind the fd with the host variables. If the host family is AF_UNIX, 
 * delete the socket at the host.sun_path, recreate a socket and add the socket
 * path to the SocketsHandler _socketsToRemove vector.
 * @param The fd to bind, should be a socket fd.
 * @param host The host whose address will be used to bind the socket.
 * @return 0 on success, -1 on error with an error message printed in the terminal.
 */
int	SocketsHandler::bindFdToHost(int fd, const Host& host)
{
	const sockaddr	*addr;

	const sa_family_t	family = host.getFamily();

	try
	{
		const socklen_t	addrLen = host.getAddrInfo(&addr);
	
		if (family == AF_UNIX)
			return (bindUnixSocket(fd, addr, addrLen, _socketsToRemove));
		return (checkError(bind(fd, addr, addrLen), -1, "bind() : "));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (-1);
	}
}

/**
 * @brief Remove the sockets at sun_path.
 * @param sun_path The path of the socket in the file system.
 * @return If the sockets is removed or there is no sockets, returns 0. On
 * error, return -1 and print an error message in the terminal.
 */
static int		removeSocketIfExists(const char sun_path[108])
{
	struct stat fileInfos;

	if (stat(sun_path, &fileInfos) == -1)
	{
		if (errno == ENOENT)
			return (0);
		std::cerr << "stat() : " << strerror(errno) << std::endl;
		return (-1);
	}
	if ((fileInfos.st_mode & S_IFMT) != S_IFSOCK)
	{
		std::cerr << "Error : The file entered for the unix socket exists and isn't a unix socket file" << std::endl;
		return (-1);
	}
	if (std::remove(sun_path) != 0)
	{
		std::cerr << "remove () : " << strerror(errno) << std::endl;
		return (-1);
	}
	return (0);
}

/**
 * @brief Remove a preexisting socket, call bind and add the socket path to the
 * socketsToRemove vector.
 * @param fd The fd to bind, it should be a socket fd.
 * @param addr A pointer on the sockaddr structure who describe the address to
 * listen to.
 * @param addrLen The size of the addr structure (!= sizeof(sockaddr))
 * @param socketsToRemove The vector in which the socket path will be added
 * if the host family is AF_UNIX.
 * @throw It should not throw because the socketsToRemove vector should have a
 * preallocated size big enough. If the vector as been wrongly initialize it can
 * throw an std::bad_alloc.
 * @return 0 on success, -1 on eror with an error message printed in the terminal.
 */
static int	bindUnixSocket(int fd, const sockaddr *addr, socklen_t addrLen, std::vector<std::string> &socketsToRemove)
{
	const sockaddr_un	*addrUnix;

	addrUnix = (const sockaddr_un *)addr;
	if (removeSocketIfExists(addrUnix->sun_path) == -1)
		return (-1);
	if (checkError(bind(fd, addr, addrLen), -1, "bind() : "))
		return (-1);
	socketsToRemove.push_back(addrUnix->sun_path);
	return (0);
}