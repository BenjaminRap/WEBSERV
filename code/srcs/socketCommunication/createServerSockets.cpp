#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "socketCommunication.hpp"
#include "SocketsHandler.hpp"

/**
 * @brief Print the message : 'Can't listen to server : host:port'.
 */
static void	printCreateServerSocketError(const Host &host)
{
	std::cerr << "can't listen to server ";
	if (host.family == AF_INET)
	{
		std::cerr << host.addr.ipv4.sin_addr.s_addr;
		std::cerr << ":" << host.addr.ipv4.sin_port << std::endl;
	}
	else
	{
		for (size_t i = 0; i < 16; i++)
		{
			std::cerr << host.addr.ipv6.sin6_addr.__in6_u.__u6_addr8[i];
		}
		std::cerr << ":" << host.addr.ipv6.sin6_port << std::endl;
	}
}


/**
 * @brief Bind the fd to the address, port and family specified in the host.
 * @param fd The fd of the socket to bind the address to.
 * @param host The structure that contains, the address, port and family.
 * @return Return 0 on success and -1 on error with an error message.
 */
static int	bindToAddress(int fd, const Host &host)
{
	const sockaddr		*addr;
	socklen_t			addrLen;

	switch (host.family)
	{
	case AF_INET:
		addr = (const sockaddr *)&host.addr.ipv4;
		addrLen = sizeof(sockaddr_in);
		break ;
	case AF_INET6:
		addr = (const sockaddr *)&host.addr.ipv6;
		addrLen = sizeof(sockaddr_in6);
		break ;
	case AF_UNIX:
		addr = (const sockaddr *)&host.addr.unixAddr;
		addrLen = sizeof(sockaddr_un);
		break ;
	default:
		std::cerr << "bindToAddress called with an unsupported or invalid family" << std::endl;
		return (-1);
	}
	return (checkError(bind(fd, addr, addrLen), -1, "bind() : "));
}

/**
 * @brief Create a server socket, a socket used only to listen to connection creation request.
 * It create a socket, bind the host, port and family(IPV4 or IPV6), set the socket to
 * listen and return the fd.
 * If an error occur, the socket will be closed and the function return -1.
 * @param Host The configuration of the socket host (sockaddr).
 * @param maxConnection The max number of connection this socket will listen to.
 * @param reuseAddr A boolean to determine if the address can be reused just after
 * the socket being closed. Otherwise there is a delay (TIME_WAIT).
 */
static int	createServerSocket(const Host &host, int maxConnection, bool reuseAddr)
{
	int			fd;

	fd = socket(host.family, SOCK_STREAM, 0);
	if (checkError(fd, -1, "socket() : ") == -1)
		return (-1);
	if (setReusableAddr(fd, reuseAddr) == -1
		|| (host.family == AF_INET6 && setIPV6Only(fd, true) == -1)
		|| bindToAddress(fd, host) == -1
		|| checkError(listen(fd, maxConnection), -1, "listen() : ") == -1)
	{
		checkError(close(fd), -1, "close() : ");
		return (-1);
	}
	return (fd);
}

/**
 * @brief Create a server socket for each host of the conf variable.
 * It does not crash on error, instead print an error message with the function name,
 * a error message depending on errno and the server informations.
 * @param conf The configuration, it will not be changed.
 * @param socketsHandler The class that will be used to add sockets to the epoll
 * interest list.
 */
void	createAllServerSockets(const Configuration &conf, SocketsHandler &socketsHandler)
{
	int				fd;
	const uint32_t	events = EPOLLIN | EPOLLET;

	for (Configuration::const_iterator ci = conf.begin(); ci != conf.end(); ci++)
	{
		const Host	&host = (*ci).first;

		fd = createServerSocket(host, conf.maxConnectionBySocket, conf.reuseAddr);
		if (fd == -1)
			printCreateServerSocketError(host);
		else if (socketsHandler.addFdToListeners(fd, acceptConnection, (void *)&socketsHandler, events) == -1)
		{
			close(fd);
			printCreateServerSocketError(host);
		}
	}
}