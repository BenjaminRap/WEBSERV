#include "SocketData.hpp"

SocketData::SocketData(int fd, void *data, void (&callback)(int fd, void *data))
	: _fd(fd), _data(data), _callback(callback)
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