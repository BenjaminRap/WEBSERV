#include <fcntl.h>                  // for FD_CLOEXEC, O_NONBLOCK
#include <stddef.h>                 // for NULL
#include <stdint.h>                 // for uint32_t
#include <stdexcept>                // for runtime_error, invalid_argument
#include <string>                   // for basic_string

#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "EPollHandler.hpp"         // for EPollHandler
#include "socketCommunication.hpp"  // for addFlagsToFd, closeFdAndPrintError

AFdData::AFdData(int fd, EPollHandler& ePollHandler, AFdDataChilds type, uint32_t events) :
	_fd(fd),
	_ePollHandler(&ePollHandler),
	_isActive(true),
	_type(type)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");

	if (addFlagsToFd(_fd, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw std::runtime_error("AFdData: Can't apply flags to fd");
	if (_ePollHandler->addFdToEpoll(*this, events) == -1)
		throw std::runtime_error("Can't add the fd to epoll !");

}

AFdData::AFdData(int fd, AFdDataChilds type) :
	_fd(fd),
	_ePollHandler(NULL),
	_type(type)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");

	if (addFlagsToFd(_fd, FD_CLOEXEC) == -1)
		throw std::runtime_error("AFdData: Can't apply flags to fd");
}

AFdData::~AFdData(void)
{
	if (_ePollHandler != NULL)
		_ePollHandler->closeFdAndRemoveFromEpoll(_fd);
	else if (_fd > 0)
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
