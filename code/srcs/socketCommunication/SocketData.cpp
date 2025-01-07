#include "SocketData.hpp"

SocketData::SocketData(int fd, void (&callback)(int fd)) : _fd(fd), _callback(callback)
{
}

void	SocketData::callback() const
{
	this->_callback(this->_fd);
}

int	SocketData::getFd() const
{
	return (this->_fd);
}