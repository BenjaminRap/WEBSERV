#include <stdexcept>

#include "Configuration.hpp"

bool	Configuration::_instanciated = false;

Configuration::Configuration(void)
{
	if (_instanciated)
		throw std::logic_error("Error : Trying to instanciate a Configuration multiples times");
	_instanciated = true;

	Host								host((in_addr_t)0, (in_port_t)8080);
	std::vector<ServerConfiguration>	serverConfigurations;

	_maxEvents = 500;
	_maxConnectionBySocket = 100;
	(*this)[host] = serverConfigurations;
	(*this)[host].push_back(ServerConfiguration());
	_reuseAddr = true;
}

Configuration::~Configuration(void)
{
	_instanciated = false;
}

int	Configuration::getMaxConnectionBySocket(void) const
{
	return (_maxConnectionBySocket);
}

unsigned int	Configuration::getMaxEvents(void) const
{
	return (_maxEvents);
}

bool	Configuration::getReuseAddr(void) const
{
	return (_reuseAddr);
}