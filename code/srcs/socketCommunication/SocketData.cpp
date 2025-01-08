#include <cstddef>
#include <list>
#include <sys/epoll.h>
#include <unistd.h>

#include "SocketData.hpp"
#include "socketCommunication.hpp"

SocketData::SocketData(int fd, void *data, void (&callback)(int fd, void *data))
	: _fd(fd), _data(data), _callback(callback), _iterator(), _isIteratorSet(false)
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
	if (_isIteratorSet)
		return (this->_iterator);
	else
		throw std::logic_error("SocketData getIterator() function with a unitialized iterator");
}

void	SocketData::setIterator(std::list<SocketData>::iterator iterator)
{
	if (_isIteratorSet)
	{
		std::cerr << "Error : try setting an iterator twice" << std::endl;
		return ;
	}
	_isIteratorSet = true;
	this->_iterator = iterator;
}
