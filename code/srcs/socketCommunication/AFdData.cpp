#include <stdexcept>           // for invalid_argument, logic_error
#include <string>              // for char_traits, basic_string

#include "AFdData.hpp"         // for AFdData

class ServerConfiguration;  // lines 11-11

AFdData::AFdData(int fd) :
	_fd(fd)
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
