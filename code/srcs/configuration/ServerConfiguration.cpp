#include "ServerConfiguration.hpp"

bool	Host::operator<(const Host& host) const
{
	if (this->address == host.address)
		return (this->port < host.port);
	return (this->address < host.address);
}


Host::Host(uint32_t address, sa_family_t family, uint16_t port) :
	address(address), family(family), port(port)
{
}