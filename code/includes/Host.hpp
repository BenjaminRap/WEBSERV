#ifndef HOST_HPP
# define HOST_HPP

# include <netinet/in.h>
# include <sys/un.h>
# include <string>

/**
 * @brief This class represents the host : addresse, port and family. It will be
 * used to create server sockets.

 */
class Host
{
private:
	/**
	 * @brief The type of connection : AF_INET (IPV4),
 	 * AF_INET6 (IPV6) or AF_UNIX (unix socket) for internal connections.
	 * Depending on the value of family, the _addr union is cast as a sockarddr_in (IPV4),
	 * sockaddr_in6 (IPV6) or sockaddr_un (unix socket).
	 */
	sa_family_t		_family;
	/**
	 * @brief An union between all the structure needed by the bind() function,
	 * depending on their family.
	 */
	union sockaddr_in_u
	{
		sockaddr_in		ipv4;
		sockaddr_in6	ipv6;
		sockaddr_un		unixAddr;
	}				_addr;
	
	Host(void);

	Host&	operator=(const Host& ref);
public:
	Host(in_addr_t addrIPV4, in_port_t port);
	Host(uint8_t (&addrIPV6)[16], in_port_t port);
	Host(std::string &path);
	Host(const Host &ref);
	~Host(void);

	bool		operator<(const Host &host) const;

	socklen_t	getAddrInfo(const sockaddr ** outAddr) const;
	sa_family_t	getFamily(void) const;
};

#endif // !HOST_HPP