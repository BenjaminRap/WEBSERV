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
#include "ASocketData.hpp"          // for ASocketData
#include "Configuration.hpp"        // for Configuration
#include "EPollHandler.hpp"         // for EPollHandler
#include "Host.hpp"                 // for Host
#include "socketCommunication.hpp"  // for checkError, closeFdAndPrintError


bool	EPollHandler::_instanciated = false;

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

EPollHandler::EPollHandler(const Configuration &conf) :
	_maxEvents(conf.getMaxEvents()),
	_eventsCount(0),
	_unixSocketsToRemove(getUnixSocketCount(conf))
{
	if (EPollHandler::_instanciated == true)
		throw std::logic_error("Error : trying to instantiate a EPollHandler multiple times");
	_events = new epoll_event[conf.getMaxEvents()]();
	_epfd = epoll_create(1);
	if (checkError(_epfd, -1, "epoll_create() :"))
	{
		delete [] _events;
		throw std::exception();
	}
	EPollHandler::_instanciated = true;
}

EPollHandler::~EPollHandler()
{
	EPollHandler::_instanciated = false;
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

void	EPollHandler::closeFdAndRemoveFromEpoll(int fd)
{
	checkError(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL), -1, "epoll_ctl() : ");
	closeFdAndPrintError(fd);
}

int	EPollHandler::epollWaitForEvent()
{
	const int	nfds = epoll_wait(_epfd, _events, _maxEvents, -1);

	if (checkError(nfds, -1, "epoll_wait() : "))
		_eventsCount = 0;
	else
		_eventsCount = nfds;
	return (nfds);
}

void	EPollHandler::callSocketCallback(size_t eventIndex) const
{
	if (eventIndex >= _eventsCount)
	{
		std::cerr << "EPollHandler callSocketCallback method was called with a wrong index" << std::endl;
		return ;
	}
	const epoll_event&	fdEvent = _events[eventIndex];
	if (fdEvent.data.ptr == NULL)
		return ;
	AFdData	*fdData = static_cast<AFdData *>(fdEvent.data.ptr);

	fdData->callback(fdEvent.events);
}

int	EPollHandler::bindFdToHost(int fd, const Host& host)
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


int	EPollHandler::addFdToEpoll(AFdData& FdData, uint32_t events)
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

int	EPollHandler::addFdToList
(
	ASocketData &fdData
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
	return (0);
}

void	EPollHandler::removeFdDataFromList(std::list<ASocketData*>::iterator pos)
{
	const ASocketData*	socket = *pos;

	_socketsData.erase(pos);
	delete socket;
}
