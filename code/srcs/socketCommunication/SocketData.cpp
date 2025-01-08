#include <cstddef>
#include <list>
#include <sys/epoll.h>
#include <unistd.h>

#include "SocketData.hpp"
#include "socketCommunication.hpp"

/**
 * @brief Create a SocketData, it does not create the fd, add itself to the SocketsHandler
 * list nor add itself to the interest list.Those actions are done by the SocketsHandler.
 */
SocketData::SocketData(int fd, void *data, void (&callback)(int fd, void *data))
	: _fd(fd), _data(data), _callback(callback), _iterator(), _isIteratorSet(false)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");
}

/**
 * @brief Create a SocketData by copying ref.The ref iterator isn't copied and this
 * new SocketData need a setIterator call.
 * @param ref The SocketData to copy from.
 */
SocketData::SocketData(const SocketData& ref)
	:_fd(ref._fd), _data(ref._data), _callback(ref._callback), _iterator(), _isIteratorSet(false)
{
}

SocketData::~SocketData(void)
{
}


/**
 * @brief Execute the callback variable with the _fd and _data as parameters.
 */
void	SocketData::callback() const
{
	this->_callback(this->_fd, this->_data);
}

/**
 * @return The fd of the socket.
 */
int	SocketData::getFd() const
{
	return (this->_fd);
}

/**
 * @brief Return the iterator pointing to this object in the SocketHandler _socketsData
 * list.
 * @throw If the iterator hasn't been set with the setIterator function, throw
 * a std::logic_error.
 * @return A const reference on the iterator pointing to this object.
 */
const std::list<SocketData>::iterator	&SocketData::getIterator() const
{
	if (_isIteratorSet)
		return (this->_iterator);
	else
		throw std::logic_error("SocketData getIterator() function with a unitialized iterator");
}

/**
 * @brief Set the _iterator of this SocketData to a copy of the iterator passed
 * as argument. This function should be called if you want to remove this socket
 * later.
 * If the iterator has already been set, print an error.
 * If the SocketData pointed by the iterator isn't this class, print an error.
 * @param iterator The iterator that points to this SocketData.
 */
void	SocketData::setIterator(const std::list<SocketData>::iterator &iterator)
{
	if (_isIteratorSet)
	{
		std::cerr << "Error : trying to set an iterator twice" << std::endl;
		return ;
	}
	if (&(*iterator) != this)
	{
		std::cerr << "Error : trying to set the iterator with an iterator pointing to the wrong SocketData" << std::endl;
		return ;
	}
	_isIteratorSet = true;
	this->_iterator = iterator;
}
