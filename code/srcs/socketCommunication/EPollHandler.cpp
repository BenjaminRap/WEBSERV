#include <fcntl.h>                  // for FD_CLOEXEC
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for epoll_event, epoll_ctl, epoll_create
#include <sys/socket.h>             // for AF_UNIX, bind, sockaddr, socklen_t
#include <sys/types.h>              // for ssize_t
#include <sys/un.h>                 // for sa_family_t
#include <cstdio>                   // for NULL, size_t
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, operator<<, cerr
#include <list>                     // for list, operator!=, _List_const_ite...
#include <map>                      // for operator!=, _Rb_tree_const_iterator
#include <stdexcept>                // for logic_error
#include <string>                   // for char_traits, basic_string, string
#include <utility>                  // for pair
#include <vector>                   // for vector

#include "AFdData.hpp"              // for AFdData
#include "AEPollFd.hpp"          // for ASocketData
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
	_ePollFds(),
	_ePollFdsToRemove(),
	_epfd(-1),
	_events(NULL),
	_maxEvents(conf.getMaxEvents()),
	_unixSocketsToRemove(getUnixSocketCount(conf))
{
	if (EPollHandler::_instanciated == true)
		throw std::logic_error("Error : trying to instantiate a EPollHandler multiple times");
	_events = new epoll_event[conf.getMaxEvents()]();
	_epfd = epoll_create(1);
	if (!addFlagsToFd(_epfd, 0, FD_CLOEXEC)
		&& checkError(_epfd, -1, "epoll_create() :"))
	{
		delete [] _events;
		throw std::exception();
	}
	EPollHandler::_instanciated = true;
}

EPollHandler::~EPollHandler()
{
	EPollHandler::_instanciated = false;
	for (std::list<AEPollFd *>::const_iterator ci = _ePollFds.begin(); ci != _ePollFds.end(); ci++)
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

void	EPollHandler::removeSocketsFromRemoveList(void)
{
	for (std::list<const AEPollFd*>::const_iterator it = _ePollFdsToRemove.begin(); it != _ePollFdsToRemove.end(); it++)
	{
		const AEPollFd*	socket = *it;
		if (socket == NULL)
			throw std::logic_error("socket is NULL in the _socketsToRemove list !");
		const int			fd = socket->getFd();

		_ePollFds.erase(socket->getIterator());
		if (fd > 0)
			checkError(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL), -1, "epoll_ctl() : ");
		delete socket;
	}
	_ePollFdsToRemove.clear();
}

bool	EPollHandler::handleIOEvents(void)
{
	const int	nfds = epoll_wait(_epfd, _events, _maxEvents, -1);

	if (checkError(nfds, -1, "epoll_wait() : "))
		return (false);

	for (ssize_t i = 0; i < nfds; i++)
	{
		const epoll_event&	fdEvent = _events[i];
		if (fdEvent.data.ptr == NULL)
			continue ;
		static_cast<AFdData *>(fdEvent.data.ptr)->callback(fdEvent.events);
	}
	removeSocketsFromRemoveList();
	return (true);
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


bool	EPollHandler::addFdToEpoll(AFdData& fdData, uint32_t events)
{
	const int	fd = fdData.getFd();

	if (fd < 0)
		return (false);
	epoll_event	event;

	event.data.ptr = &fdData;
	event.events = events;

	const int ret = epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event);

	if (checkError(ret, -1, "epoll_ctl() :"))
		return (false);
	return (true);
}

bool	EPollHandler::addFdToList(AEPollFd &fdData)
{
	try
	{
		_ePollFds.push_front(&fdData);
	}
	catch(const std::exception& e)
	{
		std::cerr << "push_front() : " << e.what() << std::endl;
		return (false);
	}
	fdData.setIterator(_ePollFds.begin());
	return (true);
}

void	EPollHandler::clearUnixSocketsList(void)
{
	_unixSocketsToRemove.clear();
}

void	EPollHandler::addFdToRemoveList(const AEPollFd& fdData)
{
	_ePollFdsToRemove.push_back(&fdData);
}
