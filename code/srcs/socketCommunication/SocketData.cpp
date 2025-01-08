#include <cstddef>
#include <list>
#include <sys/epoll.h>
#include <unistd.h>

#include "SocketData.hpp"
#include "socketCommunication.hpp"

SocketData::SocketData(int fd, void *data, void (&callback)(int fd, void *data))
	: _fd(fd), _data(data), _callback(callback), _iterator()
{
}

void	SocketData::callback() const
{
	this->_callback(this->_fd, this->_data);
}

int	SocketData::getFd() const
{
	return (this->_fd);
}

const std::list<SocketData>::iterator	&SocketData::getIterator() const
{
	return (this->_iterator);
}

void	SocketData::setIterator(std::list<SocketData>::iterator iterator)
{
	this->_iterator = iterator;
}
