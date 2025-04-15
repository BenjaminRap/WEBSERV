#include <iostream>            // for basic_ostream, operator<<, cerr, endl
#include <list>                // for list
#include <stdexcept>           // for invalid_argument, logic_error
#include <string>              // for char_traits, basic_string
#include <vector>              // for vector

#include "ASocketData.hpp"     // for ASocketData
#include "SocketsHandler.hpp"  // for SocketsHandler

class ServerConfiguration;  // lines 11-11

ASocketData::ASocketData(int fd, SocketsHandler& socketsHandler, const std::vector<ServerConfiguration> &serverConfigurations) :
	AFdData(fd, true),
	_iterator(),
	_isIteratorSet(false),
	_socketsHandler(socketsHandler),
	_serverConfigurations(serverConfigurations)
{
}

ASocketData::~ASocketData(void)
{
	_socketsHandler.closeFdAndRemoveFromEpoll(_fd);
}

const std::list<ASocketData *>::iterator	&ASocketData::getIterator() const
{
	if (_isIteratorSet)
		return (this->_iterator);
	else
		throw std::logic_error("FdData getIterator() function with a unitialized iterator");
}

void	ASocketData::setIterator(const std::list<ASocketData *>::iterator &iterator)
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


void	ASocketData::removeFromSocketsHandler(void)
{
	if (_isIteratorSet == false)
		return ;
	_socketsHandler.removeFdDataFromList(_iterator);
}
