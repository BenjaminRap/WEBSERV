#include <stdint.h>
#include <iostream>        // for basic_ostream, operator<<, basic_ios, cerr
#include <list>            // for list
#include <stdexcept>       // logic_error
#include <string>          // for char_traits, basic_string

#include "SocketData.hpp"  // for SocketData


/**
 * @brief Create a SocketData by copying ref.The ref iterator isn't copied and this
 * new SocketData need a setIterator call.
 * @param ref The SocketData to copy from.
 */
SocketData::SocketData(const SocketData& ref) :
	_fd(ref._fd),
	_data(ref._data),
	_callback(ref._callback),
	_iterator(),
	_isIteratorSet(false)
{
}

SocketData::~SocketData(void)
{
}

/**
 * @brief Execute the callback variable with the _fd and _data as parameters.
 */
void	SocketData::callback(uint32_t events)
{
	this->_callback(*this, this->_data, events);
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
const std::list<SocketData *>::iterator	&SocketData::getIterator() const
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
void	SocketData::setIterator(const std::list<SocketData *>::iterator &iterator)
{
	if (_isIteratorSet)
	{
		std::cerr << "Error : trying to set an iterator twice" << std::endl;
		return ;
	}
	if (*iterator != this)
	{
		std::cerr << "Error : trying to set the iterator with an iterator pointing to the wrong SocketData" << std::endl;
		return ;
	}
	_isIteratorSet = true;
	this->_iterator = iterator;
}
