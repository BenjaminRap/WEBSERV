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
	std::cerr << "can't listen to server :" << host.address << ":" << host.port << std::endl;
}

/**
 * @brief Bind the fd to the address, port and family specified in the host.
 * @param fd The fd of the socket to bind the address to.
 * @param host The structure that contains, the address, port and family.
 * @return Return 0 on success and -1 on error.
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
 * @brief Set the socket SO_REUSEADDR option depending on reuseAddr.
 * @param fd The fd of the socket to set the SO_REUSEADDR option to.
 * @param reuseAddr True if the address can be reused shortly after, false otherwise.
 * Setting it to true can bring bugs but prevent the bind() : address already in use
 * error.
 * @return Return 0 on success and -1 on error, with an error message printed
 * in the terminal.
 */
static int	setReusableAddr(int fd, bool reuseAddr)
{
	int	reusable = reuseAddr ? 1 : 0;
	int	returnValue;

	returnValue = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reusable, sizeof(int));
	checkError(returnValue, -1, "setsockopt() : ");
	return (returnValue);
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

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (checkError(fd, -1, "socket() : ") == -1)
		return (-1);
	if (setReusableAddr(fd, reuseAddr) == -1
		|| checkError(bindToAddress(fd, host), -1, "bind() :") == -1
		|| checkError(listen(fd, maxConnection), -1, "listen() : ") == -1)
	{
		checkError(close(fd), -1, "close() : ");
		return (-1);
	}
	return (fd);
}

/**
 * @brief Read the fd until it receive eof or an error.It prints in the terminal
 * what it has received.
 * @param fd The fd of a socket to listen to.
 * @param data Unused data.
 */
static void	writeReceived(int fd, void *data)
{
	char	buffer[8];
	ssize_t	rd;

	(void)data;
	while (true)
	{
		rd = recv(fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);
		if (rd == -1 && errno != EAGAIN)
		{
			std::cerr << "recv() : " << strerror(errno) << std::endl;
			return ;
		}
		if (rd <= 0)
		{
			return ;
		}
		buffer[rd] = '\0';
		std::cout << buffer;
	}
}

/**
 * @brief Accept a connection request, sent to fd, add it to the epoll interest
 * list and add a SocketData node.
 * The socket that will be created has the writeReceived function as callback.
 * @param fd The fd of the serverSocket that received a connection request.
 * @param data This pointer will be casted as a socketsHandler *, and will be used
 * to add it to the epoll interest list and socketsData list.
 */
static void	acceptConnection(int fd, void *data)
{
	int				newConnectionFd;
	sockaddr_in		addr;
	socklen_t		addrLength;
	SocketsHandler	*socketsHandler;

	addrLength = sizeof(addr);
	newConnectionFd = accept(fd, (sockaddr *)&addr, &addrLength);
	if (checkError(fd, -1, "accept() : ") == -1)
		return ;
	socketsHandler = (SocketsHandler *)data;
	if (socketsHandler->addFdToListeners(newConnectionFd, writeReceived, NULL, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP) == -1)
	{
		std::cerr << "Can't accept new connection" << std::endl;
		checkError(close(newConnectionFd), -1, "close() : ");
	}
	else
		std::cout << "Accepted a new connection, fd : " << newConnectionFd << std::endl;
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