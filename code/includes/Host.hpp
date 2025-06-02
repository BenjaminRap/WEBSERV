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
	const sa_family_t	_family;
	/**
	 * @brief An union between all the structure needed by the bind() function,
	 * depending on their family.
	 */
	union sockaddr_in_u	_addr;
	/**
	 * @brief The string representation of the port number.
	 */
	const std::string	_port;
	
	Host(void);
	Host&	operator=(const Host& ref);

public:
	/**
	 * @brief Create an Host object, with an IPV4 family.
	 * @param addrIPV4 The address to listen to, in an IPV4 format
	 * @param port The port to listen to.
	 */
	Host(in_addr_t addrIPV4, in_port_t port);
	/**
	 * @brief Create an host object, with an IPV6 family.
	 * @param addrIPV4 The address to listen to, in an IPV6 format
	 * @param port The port to listen to.
	 */
	Host(const uint8_t	(&addrIPV6)[16], in_port_t port);
	/**
	 * @brief Create an host object, with an unix family.
	 * @param path The path to the unix endpoint.
	 */
	Host(const std::string &path);
	Host(const Host &ref);
	~Host(void);

	/**
	 * @brief Compare two host, first by their family, then by their addr value.
	 * @return True if this Host is inferior, false if it is equal or superior.
	 */
	bool				operator<(const Host &host) const;

	/**
	 * @brief Get the sockaddr and length of the sockaddr structure. It is useful
	 * for bind().
	 * @param outAddr The variable in which to store the address (const sockaddr *).
	 * @throw If outAddr is pointing to NULL or if this Host family is unsupported,
	 * throw a std::logic_error.
	 * @return The length of the sockaddr structure. It can
	 * be sizeof(sockaddr_in), sizeof(sockaddr_in6) or sizeof(sockaddr_un), depending
	 * on this Host family.
	 */
	socklen_t			getAddrInfo(const sockaddr ** outAddr) const;
	/**
	 * @brief Return the family of this socket, it can be AF_INET, AF_INET6 or AF_UNIX.
	 */
	sa_family_t			getFamily(void) const;
	sockaddr_in			getipv4Addr(void) const;
	sockaddr_in6		getipv6Addr(void) const;
	sockaddr_un			getunixAddr(void) const;
	const std::string&	getPort(void) const;
};

#endif // !HOST_HPP
