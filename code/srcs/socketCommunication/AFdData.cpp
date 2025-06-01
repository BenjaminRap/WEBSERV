#include <fcntl.h>                  // for FD_CLOEXEC, O_NONBLOCK
#include <stddef.h>                 // for NULL
#include <stdint.h>                 // for uint32_t
#include <sys/types.h>              // for ssize_t
#include <stdexcept>                // for runtime_error, invalid_argument
#include <string>                   // for basic_string

#include "AFdData.hpp"              // for AFdData, AFdDataChilds
#include "EPollHandler.hpp"         // for EPollHandler
#include "socketCommunication.hpp"  // for addFlagsToFd, closeFdAndPrintError

AFdData::AFdData(int fd, AFdDataChilds type, bool isBlocking) :
	_fd(-1),
	_isActive(true),
	_isBlocking(isBlocking),
	_type(type)
{
	setFd(fd);
}

AFdData::AFdData(AFdDataChilds type) :
	_fd(-1),
	_isActive(true),
	_isBlocking(true),
	_type(type)
{
}
AFdData::~AFdData(void)
{
	if (_fd < 0)
		return ;
	closeFdAndPrintError(_fd);
}

int	AFdData::getFd() const
{
	return (this->_fd);
}

bool	AFdData::getIsBlocking(void) const
{
	return (_isBlocking);
}

bool	AFdData::getIsActive(void) const
{
	return (_isActive);
}

AFdData::AFdDataChilds	AFdData::getType(void) const
{
	return (_type);
}

void	AFdData::setFd(int newFd)
{
	if (_fd > 0)
		throw std::logic_error("AFdData::setFd called with a fd already set !");
	if (newFd < 0)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");

	if (!addFlagsToFd(newFd, 0, FD_CLOEXEC))
		throw std::runtime_error("AFdData: Can't apply FD_CLOEXEC to fd");
	_fd = newFd;
}
