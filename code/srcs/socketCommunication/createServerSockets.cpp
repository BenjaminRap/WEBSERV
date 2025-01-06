#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <string.h>
#include <iostream>
#include <errno.h>
#include <unistd.h>

#include "Configuration.hpp"

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
	sockaddr_in	addr;

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		std::cerr << "socket() : ";
		return (-1);
	}
	bzero((char *)&addr, sizeof(sockaddr_in));
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(serverConf.port);
	if (bind(fd, (const sockaddr *)&addr, sizeof(addr)) < 0)
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

/**
 * @brief Create a server socket for each server configuration of the conf variable.
 * It does not crash on error, instead print an error message with the function name,
 * and the server informations.
 * @param conf The configuration, it will not be changed.
 * @param epfd The epoll fd, used to add socket to its interest list.
 */
void	createAllServerSockets(const Configuration &conf, int epfd, std::vector<int> &fds)
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
		event.data.fd = fd;
		event.events = EPOLLIN | EPOLLOUT | EPOLLET;
		if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event) == -1)
		{
			close(fd);
			std::cerr << "epoll_ctl()";
			printCreateServerSocketError(serverConf);
			continue ;
		}
		fds.push_back(fd);
	}
}