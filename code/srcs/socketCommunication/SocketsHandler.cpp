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
	int	fd;

	for (std::vector<SocketData>::const_iterator ci = _socketsData.begin(); ci < _socketsData.end(); ci++)
	{
		fd = (*ci).getFd();
		checkError(epoll_ctl(_epfd, EPOLL_CTL_DEL, fd, NULL), -1, "epoll_ctl() : ");
		checkError(close(fd), -1, "close() : ");
	}
	checkError(close(_epfd), -1, "close() :");
	delete [] _events;
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
		_socketsData.push_back(SocketData(fd, data, callback));
	}
	catch(const std::exception& e)
	{
		std::cerr << "push_back() : " << e.what() << std::endl;
		return (-1);
	}
	event.data.ptr = &_socketsData.back();
	event.events = events;
	if (checkError(epoll_ctl(_epfd, EPOLL_CTL_ADD, fd, &event), -1, "epoll_ctl() :") == -1)
	{
		_socketsData.pop_back();
		return (-1);
	}
	return (0);
}


void	SocketsHandler::callSocketCallback(size_t index)
{
	SocketData	*socketData;

	if (index < _eventsCount)
	{
		socketData = (SocketData *)_events[index].data.ptr;
		socketData->callback();
	}
	else
		std::cerr << "SocketsHandler callSocketCallback method was called with a wrong index" << std::endl;
}