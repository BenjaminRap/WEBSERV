#include <cstring>
#include <stdexcept>
#include <iostream>

#include "Host.hpp"
#include "socketCommunication.hpp"

/**
 * @brief Compare two host, first by their family, then by their addr value.
 * @return True if this Host is inferior, false if it is equal or superior.
 */
bool	Host::operator<(const Host& host) const
{
	if (this->_family == host._family)
		return (std::memcmp(&this->_addr, &host._addr, sizeof(sockaddr_in_u)) < 0);
	return (this->_family < host._family);
}

/**
 * @brief Create an Host object, with an IPV4 family.
 * @param addrIPV4 The address to listen to, in an IPV4 format
 * @param port The port to listen to.
 */
Host::Host(in_addr_t addrIPV4, in_port_t port) : _family(AF_INET)
{
	sockaddr_in	&addr = this->_addr.ipv4;

	bzero(&this->_addr, sizeof(sockaddr_in_u));
	addr.sin_addr.s_addr = htonl(addrIPV4);
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
}

/**
 * @brief Create an host object, with an IPV6 family.
 * @param addrIPV4 The address to listen to, in an IPV6 format
 * @param port The port to listen to.
 */
Host::Host(uint8_t	(&addrIPV6)[16], in_port_t port) : _family(AF_INET6)
{
	sockaddr_in6	&addr = this->_addr.ipv6;

	bzero(&this->_addr, sizeof(sockaddr_in_u));
	for (size_t i = 0; i < 16; i++)
	{
		addr.sin6_addr.__in6_u.__u6_addr8[i] = addrIPV6[i];
	}
	addr.sin6_family = AF_INET6;
	addr.sin6_port = htons(port);
	addr.sin6_flowinfo = 0;
	addr.sin6_scope_id = 0;
}

/**
 * @brief Create an host object, with an unix family.
 * @param path The path to the unix endpoint.
 */
Host::Host(std::string &path) : _family(AF_UNIX)
{
	sockaddr_un	&addr = this->_addr.unixAddr;

	if (path.size() >= sizeof(addr.sun_path))
		throw std::invalid_argument("The path is too long");
	bzero(&this->_addr, sizeof(sockaddr_in_u));
	addr.sun_family = AF_UNIX;
	std::memcpy(addr.sun_path, path.c_str(), path.size());
}

Host::Host(const Host& ref) : _family(ref._family), _addr(ref._addr)
{
}

Host::~Host(void)
{
}

/**
 * @brief Bind the fd to this host
 * @return 0 on success or -1 on error with an error message in the terminal.
 */
int	Host::bindFdToHost(int fd) const
{
	const sockaddr		*addr;
	socklen_t			addrLen;

	switch (_family)
	{
	case AF_INET:
		addr = (const sockaddr *)&_addr.ipv4;
		addrLen = sizeof(sockaddr_in);
		break ;
	case AF_INET6:
		addr = (const sockaddr *)&_addr.ipv6;
		addrLen = sizeof(sockaddr_in6);
		break ;
	case AF_UNIX:
		addr = (const sockaddr *)&_addr.unixAddr;
		addrLen = sizeof(sockaddr_un);
		break ;
	default:
		std::cerr << "bindToAddress called with an unsupported or invalid family" << std::endl;
		return (-1);
	}
	return (checkError(bind(fd, addr, addrLen), -1, "bind() : "));
}

/**
 * @brief Return the family of this socket, it can be AF_INET, AF_INET6 or AF_UNIX.
 */
sa_family_t	Host::getFamily(void) const
{
	return (_family);
}