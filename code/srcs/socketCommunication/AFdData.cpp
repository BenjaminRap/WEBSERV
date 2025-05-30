#include <fcntl.h>                  // for FD_CLOEXEC, O_NONBLOCK
#include <stddef.h>                 // for NULL
#include <stdint.h>                 // for uint32_t
#include <sys/types.h>              // for ssize_t
#include <stdexcept>                // for runtime_error, invalid_argument
#include <string>                   // for basic_string

#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "EPollHandler.hpp"         // for EPollHandler
#include "socketCommunication.hpp"  // for addFlagsToFd, closeFdAndPrintError

AFdData::AFdData(int fd, EPollHandler& ePollHandler, AFdDataChilds type, uint32_t events) :
	_fd(-1),
	_ePollHandler(&ePollHandler),
	_isActive(true),
	_type(type),
	_eventIndex(-1),
	_addedToEPoll(false)
{
	if (events == 0)
		throw std::logic_error("AfdData constructor called with events set to 0");
	setFd(fd, events);
}

AFdData::AFdData(EPollHandler& ePollHandler, AFdDataChilds type) :
	_fd(-1),
	_ePollHandler(&ePollHandler),
	_isActive(true),
	_type(type),
	_eventIndex(-1),
	_addedToEPoll(false)
{
}

AFdData::AFdData(int fd, AFdDataChilds type) :
	_fd(-1),
	_ePollHandler(NULL),
	_type(type),
	_eventIndex(-1),
	_addedToEPoll(false)
{
	setFd(fd, 0);
}

AFdData::~AFdData(void)
{
	if (_fd < 0)
		return ;
	if (_addedToEPoll)
		_ePollHandler->closeFdAndRemoveFromEpoll(_fd, _eventIndex);
	closeFdAndPrintError(_fd);
}

int	AFdData::getFd() const
{
	return (this->_fd);
}

bool	AFdData::getIsBlocking(void) const
{
	return (_ePollHandler != NULL);
}

bool	AFdData::getIsActive(void) const
{
	return (_isActive);
}

AFdDataChilds	AFdData::getType(void) const
{
	return (_type);
}

void	AFdData::setEventIndex(ssize_t eventIndex)
{
	_eventIndex = eventIndex;
}

void	AFdData::setFd(int fd, uint32_t events)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");
	if (events != 0 && !_ePollHandler)
		throw std::logic_error("setFd called with events different to 0, but the instance hasn't be created with the right constructor");

	_fd = fd;
	if (!addFlagsToFd(_fd, O_NONBLOCK, FD_CLOEXEC))
		throw std::runtime_error("AFdData: Can't apply flags to fd");
	if (events == 0)
		return ;
	if (_ePollHandler->addFdToEpoll(*this, events) == -1)
		throw std::runtime_error("Can't add the fd to epoll !");
	_addedToEPoll = true;
}
