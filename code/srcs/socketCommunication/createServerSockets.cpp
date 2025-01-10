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
 * @brief Create a server socket, a socket used only to listen to connection creation request.
 * It create a socket, bind the host, port and family(IPV4 or IPV6), set the socket to
 * listen and return the fd.
 * If an error occur, the socket will be closed and the function return -1.
 * @param Host The configuration of the socket host (sockaddr).
 * @param maxConnection The max number of connection this socket will listen to.
 * @param reuseAddr A boolean to determine if the address can be reused just after
 * the socket being closed. Otherwise there is a delay (TIME_WAIT).
 */
static int	createServerSocket(const Host &host, int maxConnection, bool reuseAddr, SocketsHandler &socketsHandler)
{
	int			fd;
	sa_family_t	family;

	family = host.getFamily();
	fd = socket(family, SOCK_STREAM, 0);
	if (checkError(fd, -1, "socket() : ") == -1)
		return (-1);
	if (setReusableAddr(fd, reuseAddr) == -1
		|| (family == AF_INET6 && setIPV6Only(fd, true) == -1)
		|| socketsHandler.bindFdToHost(fd, host) == -1
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
	const uint32_t	events = EPOLLIN | EPOLLET | EPOLLERR | EPOLLHUP;

	for (Configuration::const_iterator ci = conf.begin(); ci != conf.end(); ci++)
	{
		const Host	&host = (*ci).first;

		fd = createServerSocket(host, conf.getMaxConnectionBySocket(), conf.getReuseAddr(), socketsHandler);
		if (fd == -1)
			continue ;
		if (socketsHandler.addFdToListeners(fd, acceptConnection, (void *)&socketsHandler, events) == -1)
			close(fd);
	}
}