#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for epoll_event, epoll_ctl, epoll_create
#include <sys/socket.h>             // for AF_UNIX, bind, sockaddr, socklen_t
#include <sys/un.h>                 // for sa_family_t
#include <cstdio>                   // for size_t, NULL
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, operator<<, cerr
#include <list>                     // for list, operator!=, _List_const_ite...
#include <map>                      // for operator!=, _Rb_tree_const_iterator
#include <stdexcept>                // for logic_error
#include <string>                   // for char_traits, basic_string, string
#include <utility>                  // for pair
#include <vector>                   // for vector

#include "AFdData.hpp"              // for AFdData
#include "Configuration.hpp"        // for Configuration
#include "Host.hpp"                 // for Host
#include "SocketsHandler.hpp"       // for SocketsHandler
#include "socketCommunication.hpp"  // for checkError, closeFdAndPrintError

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

SocketsHandler::SocketsHandler(const Configuration &conf) :
	_maxEvents(conf.getMaxEvents()),
	_eventsCount(0),
	_unixSocketsToRemove(getUnixSocketCount(conf))
{
	if (SocketsHandler::_instanciated == true)
		throw std::logic_error("Error : trying to instantiate a SocketsHandler multiple times");
	_events = new epoll_event[conf.getMaxEvents()]();
	_epfd = epoll_create(1);
	if (checkError(_epfd, -1, "epoll_create() :"))
	{
		delete [] _events;
		throw std::exception();
	}
	SocketsHandler::_instanciated = true;
}

SocketsHandler::~SocketsHandler()
{
	SocketsHandler::_instanciated = false;
	for (std::list<ASocketData *>::const_iterator ci = _socketsData.begin(); ci != _socketsData.end(); ci++)
	{
		delete (*ci);
	}
	closeFdAndPrintError(_epfd);
	delete [] _events;
	for (std::vector<std::string>::iterator ci = _unixSocketsToRemove.begin(); ci != _unixSocketsToRemove.end(); ci++)
	{
		removeUnixSocketIfExists((*ci).c_str());
	}
}

void	SocketsHandler::closeFdAndRemoveFromEpoll(int fd)
{
	checkError(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL), -1, "epoll_ctl() : ");
	closeFdAndPrintError(fd);
}

int	SocketsHandler::epollWaitForEvent()
{
	const int	nfds = epoll_wait(_epfd, _events, _maxEvents, -1);

	if (checkError(nfds, -1, "epoll_wait() : "))
		_eventsCount = 0;
	else
		_eventsCount = nfds;
	return (nfds);
}

void	SocketsHandler::callSocketCallback(size_t eventIndex) const
{
	if (eventIndex >= _eventsCount)
	{
		std::cerr << "SocketsHandler callSocketCallback method was called with a wrong index" << std::endl;
		return ;
	}
	if (!(_events[eventIndex].events & (EPOLLIN | EPOLLOUT)))
		return ;
	ASocketData	*fdData = static_cast<ASocketData *>(_events[eventIndex].data.ptr);

	fdData->callback(_events[eventIndex].events);
}

bool	SocketsHandler::closeIfConnectionStopped(size_t eventIndex)
{
	if (eventIndex >= _eventsCount)
		throw std::logic_error("closeIfConnectionStopped was called with wrong index");
	if ((_events[eventIndex].events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR)) == false)
		return (false);
	const ASocketData * const	fdData = static_cast<ASocketData *>(_events[eventIndex].data.ptr);

	_socketsData.erase(fdData->getIterator());
	delete fdData;
	return (true);
}

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


int	SocketsHandler::addFdToEpoll(ASocketData& FdData, uint32_t events)
{
	epoll_event	event;

	event.data.ptr = &FdData;
	event.events = events;
	if (checkError(epoll_ctl(_epfd, EPOLL_CTL_ADD, FdData.getFd(), &event), -1, "epoll_ctl() :"))
	{
		_socketsData.pop_front();
		return (-1);
	}
	return (0);
}

int	SocketsHandler::addFdToList
(
	ASocketData &fdData,
	uint32_t events
)
{
	try
	{
		_socketsData.push_front(&fdData);
	}
	catch(const std::exception& e)
	{
		std::cerr << "push_front() : " << e.what() << std::endl;
		return (-1);
	}
	fdData.setIterator(_socketsData.begin());
	if (addFdToEpoll(fdData, events) == -1)
	{
		_socketsData.pop_front();
		return (-1);
	}
	return (0);
}

void	SocketsHandler::removeFdDataFromList(std::list<ASocketData*>::iterator pos)
{
	const ASocketData*	socket = *pos;

	_socketsData.erase(pos);
	delete socket;
}
