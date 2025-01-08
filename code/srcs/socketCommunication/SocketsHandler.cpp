#include <unistd.h>

#include "SocketsHandler.hpp"
#include "socketCommunication.hpp"

SocketsHandler::SocketsHandler(int maxEvents) : _maxEvents(maxEvents), _eventsCount(0)
{
	_events = new epoll_event[maxEvents]();
	_epfd = checkError(epoll_create(1), -1, "epoll_create() :");
	if (_epfd == -1)
	{
		throw std::exception();
		return ;
	}
}

SocketsHandler::~SocketsHandler()
{
	for (std::list<SocketData>::const_iterator ci = _socketsData.begin(); ci != _socketsData.end(); ci++)
	{
		closeSocket(*ci);
	}
	checkError(close(_epfd), -1, "close() :");
	delete [] _events;
}

void	SocketsHandler::closeSocket(const SocketData &socketData)
{
	int								fd;

	fd = socketData.getFd();
	checkError(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL), -1, "epoll_ctl() : ");
	checkError(close(fd), -1, "close() : ");
}

int	SocketsHandler::epollWaitForEvent()
{
	int	nfds;

	nfds = epoll_wait(_epfd, _events, _maxEvents, -1);
	if (nfds == -1)
		_eventsCount = 0;
	else
		_eventsCount = nfds;
	return (nfds);
}

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

void	SocketsHandler::callSocketCallback(size_t eventIndex)
{
	if (eventIndex >= _eventsCount)
	{
		std::cerr << "SocketsHandler callSocketCallback method was called with a wrong index" << std::endl;
		return ;
	}
	SocketData	&socketData = *(static_cast<SocketData *>(_events[eventIndex].data.ptr));
	socketData.callback();
}


bool	SocketsHandler::closeIfConnectionStopped(size_t eventIndex)
{
	if (eventIndex >= _eventsCount)
	{
		std::cerr << "SocketsHandler closeIfConnectionStopped method was called with a wrong index" << std::endl;
		return (false);		
	}
	if ((_events[eventIndex].events & (EPOLLHUP | EPOLLRDHUP)) == false)
		return (false);
	const SocketData	&socketData = *(static_cast<SocketData *>(_events[eventIndex].data.ptr));
	closeSocket(socketData);
	try
	{
		_socketsData.erase(socketData.getIterator());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	std::cout << "A connection stopped" << std::endl;
	return (true);
}