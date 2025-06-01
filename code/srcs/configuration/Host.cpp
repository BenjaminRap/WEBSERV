#include <arpa/inet.h>   // for htons, htonl
#include <netinet/in.h>  // for sockaddr_in6, sockaddr_in, in_port_t, in6_addr
#include <sys/socket.h>  // for AF_INET, AF_INET6, AF_UNIX, socklen_t
#include <sys/un.h>      // for sockaddr_un, sa_family_t
#include <stdint.h>      // for uint8_t
#include <cstring>       // for memcmp, memcpy, NULL, size_t, memset
#include <stdexcept>     // for logic_error, invalid_argument
#include <string>        // for basic_string, string

#include "Host.hpp"      // for Host

std::string uint16toString(u_int16_t nb);

bool	Host::operator<(const Host& host) const
{
	if (this->_family == host._family)
		return (std::memcmp(&this->_addr, &host._addr, sizeof(sockaddr_in_u)) < 0);
	return (this->_family < host._family);
}

Host::Host(in_addr_t addrIPV4, in_port_t port) :
	_family(AF_INET),
	_addr(),
	_port(uint16toString(port))
{
	sockaddr_in	&addr = this->_addr.ipv4;

	std::memset(&this->_addr, '\0', sizeof(sockaddr_in_u));
	addr.sin_addr.s_addr = htonl(addrIPV4);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
}

Host::Host(const uint8_t	(&addrIPV6)[16], in_port_t port) :
	_family(AF_INET6),
	_addr(),
	_port(uint16toString(port))
{
	sockaddr_in6	&addr = this->_addr.ipv6;

	std::memset(&this->_addr, '\0', sizeof(sockaddr_in_u));
	for (size_t i = 0; i < 16; i++)
	{
		addr.sin6_addr.__in6_u.__u6_addr8[i] = addrIPV6[i];
	}
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_flowinfo = 0;
	addr.sin6_scope_id = 0;
}

Host::Host(const std::string &path) :
	_family(AF_UNIX),
	_addr(),
	_port()
{
	sockaddr_un	&addr = this->_addr.unixAddr;

	if (path.size() >= sizeof(addr.sun_path))
		throw std::invalid_argument("The path is too long");
	std::memset(&this->_addr, '\0', sizeof(sockaddr_in_u));
	addr.sun_family = AF_UNIX;
	std::memcpy(addr.sun_path, path.c_str(), path.size());
}

Host::Host(const Host& ref) :
	_family(ref._family),
	_addr(ref._addr),
	_port(ref._port)
{
}

Host::~Host(void)
{
}

socklen_t	Host::getAddrInfo(const sockaddr ** outAddr) const
{
	if (outAddr == NULL)
		throw std::logic_error("Host::getAddrInfo called with a pointer on NULL");
	switch (_family)
	{
	case AF_INET:
		*outAddr = (const sockaddr *)&_addr.ipv4;
		return (sizeof(sockaddr_in));
	case AF_INET6:
		*outAddr = (const sockaddr *)&_addr.ipv6;
		return (sizeof(sockaddr_in6));
	case AF_UNIX:
		*outAddr = (const sockaddr *)&_addr.unixAddr;
		return (sizeof(sockaddr_un));
	default:
		throw std::logic_error("A host instance has an unsupported or wrong family");
	}
}

sa_family_t	Host::getFamily(void) const
{
	return (_family);
}

sockaddr_in Host::getipv4Addr(void) const
{
	if (_family != AF_INET)
		throw std::logic_error("Host::getipv4Addr called on a non IPV4 host");
	return (_addr.ipv4);
}

sockaddr_in6 Host::getipv6Addr(void) const
{
	if (_family != AF_INET6)
		throw std::logic_error("Host::getipv6Addr called on a non IPV6 host");
	return (_addr.ipv6);
}

sockaddr_un Host::getunixAddr(void) const
{
	if (_family != AF_UNIX)
		throw std::logic_error("Host::getunixAddr called on a non UNIX host");
	return (_addr.unixAddr);
}

const std::string&	Host::getPort(void) const
{
	if (_family == AF_UNIX)
		throw std::logic_error("Host:getPort calleed with a unix socket");
	return (_port);
}
