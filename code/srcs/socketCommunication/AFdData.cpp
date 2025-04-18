#include <iostream>            // for basic_ostream, operator<<, cerr, endl
#include <list>                // for list
#include <stdexcept>           // for invalid_argument, logic_error
#include <string>              // for char_traits, basic_string
#include <vector>              // for vector

#include "AFdData.hpp"         // for AFdData
#include "SocketsHandler.hpp"  // for SocketsHandler

class ServerConfiguration;  // lines 11-11

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
	_socketsHandler.closeFdAndRemoveFromEpoll(_fd);
}

int	AFdData::getFd() const
{
	return (this->_fd);
}

const std::list<AFdData *>::iterator	&AFdData::getIterator() const
{
	if (_isIteratorSet)
		return (this->_iterator);
	else
		throw std::logic_error("FdData getIterator() function with a unitialized iterator");
}

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
