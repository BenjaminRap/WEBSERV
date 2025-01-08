#include <unistd.h>

#include "SocketsHandler.hpp"
#include "socketCommunication.hpp"

/**
 * @brief Create a SocketHandler, allocate the event array and create the epoll fd
 * @param maxEvents The size of the events array
 * @throw Throw an error if the allocation or epoll_create failed.
 */
SocketsHandler::SocketsHandler(unsigned int maxEvents) : _maxEvents(maxEvents), _eventsCount(0)
{
	_events = new epoll_event[maxEvents]();
	_epfd = checkError(epoll_create(1), -1, "epoll_create() :");
	if (_epfd == -1)
	{
		throw std::exception();
		return ;
	}
}

/**
 * @brief Free the events array, close the sockets and close the epoll fd.
 */
SocketsHandler::~SocketsHandler()
{
	for (std::list<SocketData>::const_iterator ci = _socketsData.begin(); ci != _socketsData.end(); ci++)
	{
		closeSocket((*ci).getFd());
	}
	checkError(close(_epfd), -1, "close() :");
	delete [] _events;
}

/**
 * @brief Close a socket and remove if from the epoll interest list. It does not
 * remove it from The _socketsData list.
 * @param fd The fd of the socket to close
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
	int	nfds;

	nfds = epoll_wait(_epfd, _events, _maxEvents, -1);
	if (nfds == -1)
		_eventsCount = 0;
	else
		_eventsCount = nfds;
	return (nfds);
}

/**
 * @brief Add the fd to the epoll listener and set the event events and data.ptr
 * with a SocketData.
 * @param fd The fd of the socket to add to listeners, it will also be passed
 * to the callback function.
 * @param callback It is used as a parameter of the SocketData, when the epoll_wait
 * see a read/write, we can get the SocketData and callback from the event.data.ptr.
 * @param data A data that will be passed to the callback
 * @param the events used to create the epoll_event variable
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
 * @brief Call the callback of the socket, in the epoll events at eventIndex
 * @param eventIndex The index of the event to check, [0, eventCount] where eventCount 
 * is the result of epoll_wait or epollWaitForEvent
 */
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

/**
 * @brief If the socket at eventIndex has an EPOLLHUP or EPOLLRDHUP event, close it
 * and remove it from the _socketsData list.
 * @param eventIndex The index of the event to check, [0, eventCount] where eventCount 
 * is the result of epoll_wait or epollWaitForEvent
 * @return true if the connection is closed, false otherwise.
 */
bool	SocketsHandler::closeIfConnectionStopped(size_t eventIndex)
{
	int			fd;
	SocketData	*socketData;

	if (eventIndex >= _eventsCount)
	{
		std::cerr << "SocketsHandler closeIfConnectionStopped method was called with a wrong index" << std::endl;
		return (false);		
	}
	if ((_events[eventIndex].events & (EPOLLHUP | EPOLLRDHUP)) == false)
		return (false);
	socketData = static_cast<SocketData *>(_events[eventIndex].data.ptr);
	fd = socketData->getFd();
	closeSocket(fd);
	std::cout << "A connection stopped, fd : " << fd << std::endl;
	try
	{
		_socketsData.erase(socketData->getIterator());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	return (true);
}