#include <fcntl.h>			   // for O_NONBLOCK, FD_CLOEXEC
#include <stdexcept>           // for invalid_argument, logic_error
#include <string>              // for char_traits, basic_string

#include "AFdData.hpp"         // for AFdData
#include "EPollHandler.hpp"	   // for EPollHandler

class ServerConfiguration;  // lines 11-11

AFdData::AFdData(int fd, bool isBlocking, EPollHandler& ePollHandler) :
	_fd(fd),
	_isBlocking(isBlocking),
	_ePollHandler(ePollHandler)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");

	const int	flags = _isBlocking ? O_NONBLOCK | FD_CLOEXEC : FD_CLOEXEC;
	if (addFlagsToFd(_fd, flags) == -1)
		throw std::runtime_error("AFdData: Can't apply flags to fd");
}

AFdData::~AFdData(void)
{
	if (_isBlocking)
		_ePollHandler.closeFdAndRemoveFromEpoll(_fd);
}

int	AFdData::getFd() const
{
	return (this->_fd);
}

bool	AFdData::getIsBlocking(void) const
{
	return (_isBlocking);
}
