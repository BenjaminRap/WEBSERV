#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "SocketData.hpp"
#include "socketCommunication.hpp"

/**
 * @brief Print a message if the creation of a server socket failed
 * : Can't listen to server : host:port server_name1 servername2 ...
 */
static void	printCreateServerSocketError(const ServerConfiguration &serverConf)
{
	const std::vector<std::string>	&server_names = serverConf.serverNames;
	const Host						&host = serverConf.host;

	std::cerr << "can't listen to server :" << host.address << ":" << host.port;
	for (std::vector<std::string>::const_iterator ci = server_names.begin(); ci < server_names.end(); ci++)
	{
		std::cerr << " " << *ci;
	}
	std::cerr << std::endl;
}

/**
 * @brief Bind the fd to the host, port and family specified in the serverConf
 */
static int	bindToAddress(int fd, const Host &host)
{
	sockaddr_in	addr;

	bzero((char *)&addr, sizeof(sockaddr_in));
	addr.sin_addr.s_addr = htonl(host.address);
	addr.sin_family = host.family;
	addr.sin_port = htons(host.port);
	return (bind(fd, (const sockaddr *)&addr, sizeof(addr)));
}

/**
 * @brief Create a server socket, a socket used only to listen to connection creation request.
 * It create a socket, bind the host, port and family(IPV4 or IPV6), set the socket to
 * listen and return the fd.
 * If an error occur, the socket will be closed and the function return -1.
 * @param serverConf The configuration of a server, the socket informations will depends
 * on that value, like the host and port.
 * @param maxConnection The max number of connection this socket will listen to.
 */
static int	createServerSocket(const Host &host, int maxConnection)
{
	int			fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (checkError(fd, -1, "socket() : ") == -1)
	{
		return (-1);
	}
	if (checkError(bindToAddress(fd, host), -1, "bind() :") == -1
		|| checkError(listen(fd, maxConnection), -1, "listen() : ") == -1)
	{
		close(fd);
		return (-1);
	}
	return (fd);
}

static void	printHello(int fd)
{
	std::cout << "Hello :" << fd << std::endl;
}

static int	addFdToListeners(int fd, int epfd, std::vector<SocketData> &socketsData, uint32_t events)
{
	epoll_event	event;

	socketsData.push_back(SocketData(fd, printHello));
	event.data.ptr = &socketsData.back();
	event.events = events;
	if (checkError(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event), -1, "epoll_ctl() :") == -1)
	{
		socketsData.pop_back();
		return (-1);
	}
	return (0);
}

/**
 * @brief Create a server socket for each server configuration of the conf variable.
 * It does not crash on error, instead print an error message with the function name,
 * and the server informations.
 * @param conf The configuration, it will not be changed.
 * @param epfd The epoll fd, used to add socket to its interest list.
 */
void	createAllServerSockets(const Configuration &conf, int epfd, std::vector<SocketData> &socketsData)
{
	int			fd;

	for (Configuration::const_iterator ci = conf.begin(); ci < conf.end(); ci++)
	{
		const ServerConfiguration	&serverConf = *ci;

		fd = createServerSocket(serverConf.host, conf.maxConnectionBySocket);
		if (fd == -1)
			printCreateServerSocketError(serverConf);
		else if (addFdToListeners(fd, epfd, socketsData, EPOLLIN | EPOLLET) == -1)
		{
			close(fd);
			printCreateServerSocketError(serverConf);
		}
	}
}