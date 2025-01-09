#ifndef HOST_HPP
# define HOST_HPP

# include <netinet/in.h>
# include <sys/un.h>
# include <string>

union sockaddr_in_u
{
	sockaddr_in		ipv4;
	sockaddr_in6	ipv6;
	sockaddr_un		unixAddr;
};

struct Host
{
	sa_family_t		family;
	sockaddr_in_u	addr;
	

	Host(in_addr_t addrIPV4, in_port_t port);
	Host(uint8_t (&addrIPV6)[16], in_port_t port);
	Host(std::string &path);

	bool	operator<(const Host &host) const;
};

#endif // !HOST_HPP