#include <fcntl.h>
#include <stdint.h>          // for uint32_t
#include <iostream>          // for basic_ostream, operator<<, cerr, endl
#include <list>              // for list
#include <stdexcept>         // for logic_error
#include <string>            // for char_traits, basic_string

#include "AFdData.hpp"       // for AFdData, AFdDataChilds
#include "ASocketData.hpp"   // for ASocketData
#include "EPollHandler.hpp"  // for EPollHandler

class ServerConfiguration;  // lines 10-10

ASocketData::ASocketData
(
	int fd, 
	EPollHandler& ePollHandler,
	AFdDataChilds type,
	uint32_t events
) :
	AFdData(fd, type, true),
	_iterator(),
	_isIteratorSet(false),
	_ePollHandler(ePollHandler)
{
	initFd(events);
}

ASocketData::ASocketData
(
	EPollHandler &ePollHandler,
	AFdDataChilds type
) :
	AFdData(type),
	_iterator(),
	_isIteratorSet(false),
	_ePollHandler(ePollHandler)
{
}

ASocketData::~ASocketData(void)
{
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

void	ASocketData::removeFromEPollHandler(void)
{
	if (_isIteratorSet == false)
		return ;
	_isActive = false;
	_ePollHandler.addFdToRemoveList(*this);
}

void	ASocketData::removeFromEPollHandler(AFdData* fdData)
{
	ASocketData*	socketData = static_cast<ASocketData*>(fdData);

	socketData->removeFromEPollHandler();
}

void	ASocketData::initFd(uint32_t events)
{
	if (events == 0)
		throw std::logic_error("events set to 0 in a ASocketData !");

	if (!addFlagsToFd(_fd, O_NONBLOCK, 0))
		throw std::runtime_error("ASocketData: Can't apply flags to fd");
	if (!_ePollHandler.addFdToEpoll(*this, events))
		throw std::runtime_error("Can't add the fd to epoll !");
}

void	ASocketData::setFd(int fd, uint32_t events)
{
	AFdData::setFd(fd);
	initFd(events);
}
