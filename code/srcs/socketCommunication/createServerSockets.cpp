#include <fcntl.h>					// for fcntl, F_SETFL, O_NONBLOCK ...
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLHUP, EPOLLIN
#include <sys/socket.h>             // for listen, socket, AF_INET6, SOCK_ST...
#include <sys/un.h>                 // for sa_family_t
#include <unistd.h>                 // for close
#include <exception>                // for exception
#include <iostream>                 // for operator<<, basic_ostream, cerr
#include <map>                      // for _Rb_tree_const_iterator, operator!=
#include <utility>                  // for pair
#include <vector>                   // for vector

#include "Configuration.hpp"        // for Configuration
#include "Host.hpp"                 // for Host
#include "ServerSocketData.hpp"     // for ServerSocketData
#include "SocketsHandler.hpp"       // for SocketsHandler
#include "socketCommunication.hpp"  // for checkError, setIPV6Only, setReusa...
//
class ServerConfiguration;

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
static int	createServerSocket
(
	const Host &host,
	const Configuration	&conf,
	SocketsHandler &socketsHandler
)
{
	const sa_family_t	family = host.getFamily();
	const int			fd = socket(family, SOCK_STREAM, 0);

	if (checkError(fd, -1, "socket() : "))
		return (-1);
	if (addFlagsToFd(fd, O_NONBLOCK | FD_CLOEXEC) == -1 // set the non blocking and closing on fork flags
		|| setReusableAddr(fd, conf.getReuseAddr()) == -1 // set the address reusable without delay
		|| (family == AF_INET6 && setIPV6Only(fd, true) == -1) // set the IPV6 sockets to only listen to IPV6
		|| socketsHandler.bindFdToHost(fd, host) == -1 // bind the socket to the address
		|| checkError(listen(fd, conf.getMaxConnectionBySocket()), -1, "listen() : ")) // set the socket to listening
	{
		checkError(close(fd), -1, "close() : ");
		return (-1);
	}
	return (fd);
}

/**
 * @brief Create a server socket for each host of the conf variable.
 * It does not crash on error, instead print an error message with the function name,
 * a error message depending on errno.
 * @param conf The configuration, it will not be changed.
 * @param socketsHandler The class that will be used to add sockets to the epoll
 * interest list.
 */
void	createAllServerSockets
(
const Configuration &conf,
	SocketsHandler &socketsHandler
)
{
	const uint32_t	events = EPOLLIN | EPOLLERR | EPOLLHUP;

	for (Configuration::const_iterator ci = conf.begin(); ci != conf.end(); ci++)
	{
		const Host								&host = ci->first;
		const std::vector<ServerConfiguration>	&serverConfigurations = ci->second;
		const int								fd = createServerSocket(host, conf, socketsHandler);

		if (fd == -1)
			continue ;
		try
		{
			ServerSocketData * const serverSocketData = new ServerSocketData(fd, socketsHandler, serverConfigurations);
			if (socketsHandler.addFdToListeners(serverSocketData, events) == -1)
			{
				delete serverSocketData;
				close(fd);
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
		}
	}
}
