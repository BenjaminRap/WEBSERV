#include "SocketsHandler.hpp"

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
template <typename T>
int	SocketsHandler::addFdToListeners
(
	int fd,
	void (&callback)(const SocketData &socketData, T *data, uint32_t events),
	T &data,
	uint32_t events
)
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