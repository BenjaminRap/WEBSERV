#include "SocketData.hpp"

SocketData::SocketData(int fd, void (&callback)(int fd)) : _fd(fd), _callback(callback)
{
}

void	SocketData::callBack()
{
	this->_callback(this->_fd);
}