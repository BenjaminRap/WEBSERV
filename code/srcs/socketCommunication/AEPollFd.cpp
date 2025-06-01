#include <fcntl.h>                  // for O_NONBLOCK
#include <stdint.h>                 // for uint32_t
#include <iostream>                 // for basic_ostream, operator<<, cerr
#include <list>                     // for list
#include <stdexcept>                // for logic_error, runtime_error
#include <string>                   // for char_traits, basic_string

#include "AEPollFd.hpp"             // for AEPollFd
#include "AFdData.hpp"              // for AFdData
#include "EPollHandler.hpp"         // for EPollHandler
#include "socketCommunication.hpp"  // for addFlagsToFd

AEPollFd::AEPollFd
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

AEPollFd::AEPollFd
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

AEPollFd::~AEPollFd(void)
{
}

const std::list<AEPollFd *>::iterator	&AEPollFd::getIterator() const
{
	if (_isIteratorSet)
		return (this->_iterator);
	throw std::logic_error("AEPollFd getIterator() function with a unitialized iterator");
}

void	AEPollFd::setIterator(const std::list<AEPollFd *>::iterator &iterator)
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

void	AEPollFd::removeFromEPollHandler(void)
{
	AFdData::setFinished();
	if (_isIteratorSet == false)
		return ;
	_ePollHandler.addFdToRemoveList(*this);
}

void	AEPollFd::removeFromEPollHandler(AFdData* fdData)
{
	AEPollFd*	socketData = static_cast<AEPollFd*>(fdData);

	socketData->removeFromEPollHandler();
}

void	AEPollFd::initFd(uint32_t events)
{
	if (events == 0)
		throw std::logic_error("events set to 0 in a AEPollFd !");

	if (!addFlagsToFd(getFd(), O_NONBLOCK, 0))
		throw std::runtime_error("AEPollFd: Can't apply flags to fd");
	if (!_ePollHandler.addFdToEpoll(*this, events))
		throw std::runtime_error("Can't add the fd to epoll !");
}

void	AEPollFd::setFd(int fd, uint32_t events)
{
	AFdData::setFd(fd);
	initFd(events);
}
