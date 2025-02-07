#include <netinet/in.h>             // for sockaddr_in
#include <string.h>                 // for strerror, NULL
#include <sys/epoll.h>              // for EPOLLERR, EPOLLET, EPOLLHUP, EPOLLIN
#include <sys/socket.h>             // for accept, recv, MSG_DONTWAIT, sockl...
#include <sys/types.h>              // for ssize_t
#include <stdint.h>                  // for uint32_t
#include <cerrno>                  // for errno, EAGAIN
#include <unistd.h>                 // for close
#include <iostream>                 // for basic_ostream, operator<<, endl
#include <string>                   // for char_traits, basic_string

#include <SocketsHandler.hpp>       // for SocketsHandle"
#include "socketCommunication.hpp"  // for acceptConnection

/**
 * @brief Read the fd until it receive eof or an error.It prints in the terminal
 * what it has received.
 * @param fd The fd of a socket to listen to.
 * @param data Unused data.
 */
static void	writeReceived(int fd, void *data)
{
	(void)data;
	while (true)
	{
		char			buffer[8];

		const ssize_t	rd = recv(fd, buffer, sizeof(buffer) - 1, MSG_DONTWAIT);

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
	sockaddr_in		addr;
	socklen_t		addrLength;

	addrLength = sizeof(addr);
	const uint32_t			events = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
	const int 				newConnectionFd = accept(fd, (sockaddr *)&addr, &addrLength);
	SocketsHandler* const	socketsHandler = (SocketsHandler *)data;

	if (checkError(fd, -1, "accept() : ") == -1)
		return ;
	if (socketsHandler->addFdToListeners(newConnectionFd, writeReceived, NULL, events) == -1)
	{
		std::cerr << "Can't accept new connection" << std::endl;
		checkError(close(newConnectionFd), -1, "close() : ");
	}
	else
		std::cout << "Accepted a new connection, fd : " << newConnectionFd << std::endl;
}