#include <fcntl.h>                  // for FD_CLOEXEC
#include <stdexcept>                // for invalid_argument, logic_error
#include <string>                   // for basic_string

#include "AFdData.hpp"              // for AFdData
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

void	AFdData::setFinished(void)
{
	_isActive = false;
}

void	AFdData::setFd(int newFd)
{
	if (_fd > 0)
		throw std::logic_error("AFdData::setFd called with a fd already set !");
	if (newFd < 0)
		throw std::invalid_argument("File descriptor is invalid in the AFdData constructor");

	if (!addFlagsToFd(newFd, 0, FD_CLOEXEC))
		throw std::runtime_error("AFdData: Can't apply FD_CLOEXEC to fd");
	_fd = newFd;
}
