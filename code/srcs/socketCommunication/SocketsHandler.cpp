#include <sys/epoll.h>         // for epoll_event, epoll_ctl, epoll_create
#include <sys/socket.h>        // for bind, socklen_t, AF_UNIX
#include <sys/stat.h>          // for stat, S_IFMT, S_IFSOCK
#include <sys/un.h>            // for sockaddr_un, sa_family_t
#include <unistd.h>            // for close
#include <cerrno>              // for errno, ENOENT
#include <cstring>             // for strerror
#include <cstdio>              // for size_t, remove, NULL
#include <exception>           // for exception
#include <iostream>            // for basic_ostream, operator<<, endl, cerr
#include <list>                // for list, operator!=, _List_const_iterator
#include <map>                 // for operator!=, _Rb_tree_const_iterator
#include <stdexcept>           // for logic_error
#include <string>              // for basic_string, char_traits, string
#include <utility>             // for pair
#include <vector>              // for vector

#include "Configuration.hpp"   // for Configuration
#include "socketCommunication.hpp"
#include "Host.hpp"            // for Host
#include "FdData.hpp"      // for FdData
#include "SocketsHandler.hpp"  // for SocketsHandler

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
	_unixSocketsToRemove(getUnixSocketCount(conf))
{
	if (SocketsHandler::_instanciated == true)
		throw std::logic_error("Error : trying to instantiate a SocketsHandler multiple times");
	_events = new epoll_event[conf.getMaxEvents()]();
	_epfd = checkError(epoll_create(1), -1, "epoll_create() :");
	if (_epfd == -1)
	{
		delete [] _events;
		throw std::exception();
		return ;
	}
	SocketsHandler::_instanciated = true;
}

/**
 * @brief Free the events array, close the sockets and close the epoll fd.
 */
SocketsHandler::~SocketsHandler()
{
	SocketsHandler::_instanciated = false;
	for (std::list<FdData *>::const_iterator ci = _socketsData.begin(); ci != _socketsData.end(); ci++)
	{
		closeSocket((*ci)->getFd());
		delete (*ci);
	}
	checkError(close(_epfd), -1, "close() :");
	delete [] _events;
	for (std::vector<std::string>::iterator ci = _unixSocketsToRemove.begin(); ci != _unixSocketsToRemove.end(); ci++)
	{
		removeUnixSocketIfExists((*ci).c_str());
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
	FdData	*fdData = static_cast<FdData *>(_events[eventIndex].data.ptr);

	fdData->callback(_events[eventIndex].events);
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
	const FdData * const	fdData = static_cast<FdData *>(_events[eventIndex].data.ptr);
	const int	fd = fdData->getFd();

	closeSocket(fd);
	std::cout << "A connection stopped, fd : " << fd << std::endl;
	try
	{
		_socketsData.erase(fdData->getIterator());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	delete fdData;
	return (true);
}


/**
 * @brief Bind the fd with the host variables. If the host family is AF_UNIX, 
 * delete the socket at the host.sun_path, recreate a socket and add the socket
 * path to the SocketsHandler _unixSocketsToRemove vector.
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
			return (bindUnixSocket(fd, addr, addrLen, _unixSocketsToRemove));
		return (checkError(bind(fd, addr, addrLen), -1, "bind() : "));
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (-1);
	}
}

int	SocketsHandler::addFdToListeners
(
	FdData *FdData,
	uint32_t events
)
{
	epoll_event	event;

	try
	{
		_socketsData.push_front(FdData);
	}
	catch(const std::exception& e)
	{
		delete FdData;
		std::cerr << "push_front() : " << e.what() << std::endl;
		return (-1);
	}
	FdData->setIterator(_socketsData.begin());
	event.data.ptr = FdData;
	event.events = events;
	if (checkError(epoll_ctl(_epfd, EPOLL_CTL_ADD, FdData->getFd(), &event), -1, "epoll_ctl() :") == -1)
	{
		_socketsData.pop_front();
		delete FdData;
		return (-1);
	}
	return (0);
}