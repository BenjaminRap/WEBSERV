#include <iostream>    // for basic_ostream, operator<<, cerr, endl
#include <list>        // for list
#include <stdexcept>   // for invalid_argument, logic_error
#include <string>      // for char_traits, basic_string

#include "AFdData.hpp"  // for FdData
class SocketsHandler;

/**
 * @throw Throw a std::invalid argument if the fd is inferior to 4.
 */
AFdData::AFdData(int fd, SocketsHandler& socketsHandler, const std::vector<ServerConfiguration> &serverConfigurations) :
	_fd(fd),
	_iterator(),
	_isIteratorSet(false),
	_socketsHandler(socketsHandler),
	_serverConfigurations(serverConfigurations)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");
}

AFdData::~AFdData(void)
{
}

/**
 * @return The fd of the socket.
 */
int	AFdData::getFd() const
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
const std::list<AFdData *>::iterator	&AFdData::getIterator() const
{
	if (_isIteratorSet)
		return (this->_iterator);
	else
		throw std::logic_error("FdData getIterator() function with a unitialized iterator");
}

/**
 * @brief Set the _iterator of this FdData to a copy of the iterator passed
 * as argument. This function should be called if you want to remove this socket
 * later.
 * If the iterator has already been set, print an error.
 * If the FdData pointed by the iterator isn't this class, print an error.
 * @param iterator The iterator that points to this FdData.
 */
void	AFdData::setIterator(const std::list<AFdData *>::iterator &iterator)
{
	if (_isIteratorSet)
	{
		std::cerr << "Error : trying to set an iterator twice" << std::endl;
		return ;
	}
	if (*iterator != this)
	{
		std::cerr << "Error : trying to set the iterator with an iterator pointing to the wrong FdData" << std::endl;
		return ;
	}
	_isIteratorSet = true;
	this->_iterator = iterator;
}
