#ifndef HOST_HPP
# define HOST_HPP

# include <netinet/in.h>
# include <sys/un.h>
# include <string>

struct Host
{
private:
	sa_family_t		_family;
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

	int			bindFdToHost(int fd) const;
	sa_family_t	getFamily(void) const;
};

#endif // !HOST_HPP