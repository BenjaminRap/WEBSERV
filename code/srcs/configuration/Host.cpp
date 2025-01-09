#include <cstring>
#include <stdexcept>

#include "Host.hpp"

bool	Host::operator<(const Host& host) const
{
	if (this->family == host.family)
		return (std::memcmp(&this->addr, &host.addr, sizeof(sockaddr_in_u)) < 0);
	return (this->family < host.family);
}

Host::Host(in_addr_t addrIPV4, in_port_t port) : family(AF_INET)
{
	sockaddr_in	&addr = this->addr.ipv4;

	bzero(&this->addr, sizeof(sockaddr_in_u));
	addr.sin_addr.s_addr = htonl(addrIPV4);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
}

Host::Host(uint8_t	(&addrIPV6)[16], in_port_t port) : family(AF_INET6)
{
	sockaddr_in6	&addr = this->addr.ipv6;

	bzero(&this->addr, sizeof(sockaddr_in_u));
	for (size_t i = 0; i < 16; i++)
	{
		addr.sin6_addr.__in6_u.__u6_addr8[i] = addrIPV6[i];
	}
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_flowinfo = 0;
	addr.sin6_scope_id = 0;
}

Host::Host(std::string &path) : family(AF_UNIX)
{
	sockaddr_un	&addr = this->addr.unixAddr;

	if (path.size() >= sizeof(addr.sun_path))
		throw std::invalid_argument("The path is too long");
	bzero(&this->addr, sizeof(sockaddr_in_u));
	addr.sun_family = AF_UNIX;
	std::memcpy(addr.sun_path, path.c_str(), path.size());
}