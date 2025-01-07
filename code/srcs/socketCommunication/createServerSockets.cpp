#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "Configuration.hpp"
#include "SocketData.hpp"

/**
 * @brief Print a message if the creation of a server socket failed 'Errnor Message'
 * : Can't listen to server : host:port server_name1 servername2 ...
 * Where errno message is written by the strerror function.
 */
static void	printCreateServerSocketError(const ServerConfiguration &serverConf)
{
	const std::vector<std::string>	&server_names = serverConf.serverNames;

	std::cerr << strerror(errno);
	std::cerr << ", can't listen to server :" << serverConf.host << ":" << serverConf.port;
	for (std::vector<std::string>::const_iterator ci = server_names.begin(); ci < server_names.end(); ci++)
	{
		std::cerr << " " << *ci;
	}
	std::cerr << std::endl;
}

/**
 * @brief Bind the fd to the host, port and family specified in the serverConf
 */
static int	bindToAddress(int fd, const ServerConfiguration &serverConf)
{
	sockaddr_in	addr;

	bzero((char *)&addr, sizeof(sockaddr_in));
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serverConf.port);
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
int	createServerSocket(const ServerConfiguration &serverConf, int maxConnection)
{
	int			fd;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		std::cerr << "socket() : ";
		return (-1);
	}
	if (bindToAddress(fd, serverConf) == -1)
	{
		std::cerr << "bind() : ";
		close(fd);
		return (-1);
	}
	if (listen(fd, maxConnection) == -1)
	{
		std::cerr << "listen() : ";
		close(fd);
		return (-1);
	}
	return (fd);
}

void	printHello(int fd)
{
	std::cout << "Hello :" << fd << std::endl;
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
	epoll_event	event;

	for (Configuration::const_iterator ci = conf.begin(); ci < conf.end(); ci++)
	{
		const ServerConfiguration	&serverConf = *ci;

		fd = createServerSocket(serverConf, conf.maxConnectionBySocket);
		if (fd == -1)
		{
			printCreateServerSocketError(serverConf);
			continue ;
		}
		socketsData.push_back(SocketData(fd, printHello));
		event.data.ptr = &socketsData.back();
		event.events = EPOLLIN | EPOLLOUT | EPOLLET;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1)
		{
			close(fd);
			socketsData.pop_back();
			std::cerr << "epoll_ctl()";
			printCreateServerSocketError(serverConf);
			continue ;
		}
	}
}