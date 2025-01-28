#include <netinet/in.h>             // for in_addr_t, in_port_t
#include <map>                      // for map
#include <stdexcept>                // for logic_error
#include <string>                   // for basic_string
#include <vector>                   // for vector

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

	// uint8_t								ip[16];
	// std::memset((char *)ip, '\0', sizeof(ip));
	// const Host								host(ip, (in_port_t)8080);

	const Host								host((in_addr_t)0, (in_port_t)8080);

	// const std::string						unixSocketPath("/tmp/test.sock");
	// const Host								host(unixSocketPath);
	const std::vector<ServerConfiguration>	serverConfigurations;

	_maxEvents = 500;
	_maxConnectionBySocket = 100;
	(*this)[host] = serverConfigurations;
	(*this)[host].push_back(ServerConfiguration());
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
