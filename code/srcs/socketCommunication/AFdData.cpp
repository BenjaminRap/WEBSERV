#include <stdexcept>           // for invalid_argument, logic_error
#include <string>              // for char_traits, basic_string

#include "AFdData.hpp"         // for AFdData

class ServerConfiguration;  // lines 11-11

AFdData::AFdData(int fd, bool isBlocking) :
	_fd(fd),
	_isBlocking(isBlocking)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");
}

AFdData::~AFdData(void)
{
}

int	AFdData::getFd() const
{
	return (this->_fd);
}

bool	AFdData::getIsBlocking(void) const
{
	return (_isBlocking);
}
