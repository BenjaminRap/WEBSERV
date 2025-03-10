#include <netinet/in.h>             // for in_addr_t, in_port_t
#include <map>                      // for map
#include <stdexcept>                // for logic_error
#include <string>                   // for basic_string
#include <vector>                   // for vector
#include <cstring>

#include "Configuration.hpp"        // for Configuration
#include "Host.hpp"                 // for Host
#include "ServerConfiguration.hpp"  // for ServerConfiguration

bool	Configuration::_instanciated = false;

/**
 * @brief Create a configuration object, with defaults values.
 * @throw This function can throw (std::logic_error)if there is already an
 * instance of this class.
 */
Configuration::Configuration(void)
{
	if (_instanciated)
		throw std::logic_error("Error : Trying to instanciate a Configuration multiples times");
	_maxEvents = 500;
	_maxConnectionBySocket = 100;
	_reuseAddr = true;
	Configuration::_instanciated = true;
}

/**
 * @brief Destroy this instance and allow the creation of another instance.
 */
Configuration::~Configuration(void)
{
	Configuration::_instanciated = false;
}

/**
 * @brief Return the max number of connection a socket can listen to, the others
 * will wait.
 */
int	Configuration::getMaxConnectionBySocket(void) const
{
	return (_maxConnectionBySocket);
}

/**
 * @brief Return the size of the _events array of the SocketsHandler. It determine how
 * many events the epoll_wait function can record to the user at once.
 */
unsigned int	Configuration::getMaxEvents(void) const
{
	return (_maxEvents);
}

/**
 * @brief Return a boolean, if true, the server sockets could listen just after
 * being closed, otherwise, there is a delay TIME_WAIT.
 */
bool	Configuration::getReuseAddr(void) const
{
	return (_reuseAddr);
}

std::ostream & operator<<(std::ostream & o, Configuration const & rhs)
{
	size_t	i = 1;

	for (std::map<Host, std::vector<ServerConfiguration> >::const_iterator it = rhs.begin(); it != rhs.end(); ++it)
	{
		std::cout << "Host " << i << ":" << std::endl;
		for (size_t j = 0; j < it->second.size(); j++)
		{
			std::cout << "Server " << j + 1 << ":" << std::endl;
			std::cout << it->second[j] << std::endl; 
		}
		std::cout << std::endl;
		i++;
	}
	return (o);
}
