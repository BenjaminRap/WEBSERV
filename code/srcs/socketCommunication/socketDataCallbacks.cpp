#include <sys/socket.h>
#include <SocketsHandler.hpp>
#include <unistd.h>

#include "socketCommunication.hpp"

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
void	acceptConnection(int fd, void *data)
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
	if (socketsHandler->addFdToListeners(newConnectionFd, writeReceived, NULL, EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLERR) == -1)
	{
		std::cerr << "Can't accept new connection" << std::endl;
		checkError(close(newConnectionFd), -1, "close() : ");
	}
	else
		std::cout << "Accepted a new connection, fd : " << newConnectionFd << std::endl;
}