#include <fcntl.h>			   // for O_NONBLOCK, FD_CLOEXEC
#include <stdexcept>           // for invalid_argument, logic_error
#include <string>              // for char_traits, basic_string

#include "AFdData.hpp"         // for AFdData
#include "EPollHandler.hpp"	   // for EPollHandler

class ServerConfiguration;  // lines 11-11

AFdData::AFdData(int fd, EPollHandler& ePollHandler, AFdDataChilds type) :
	_fd(fd),
	_ePollHandler(&ePollHandler),
	_isActive(true),
	_type(type)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");

	if (addFlagsToFd(_fd, O_NONBLOCK | FD_CLOEXEC) == -1)
		throw std::runtime_error("AFdData: Can't apply flags to fd");
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
