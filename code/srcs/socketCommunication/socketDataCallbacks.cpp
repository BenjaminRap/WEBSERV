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

static void	processRequests(SocketData &socketData, SocketsHandler *socketsHandler, uint32_t events)
{
	(void)socketsHandler;
	if (events & EPOLLOUT)
	{
		socketData.setCanWrite(true);
	}
	if (events & EPOLLIN)
	{

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
void	acceptConnection(SocketData &socketData, SocketsHandler *socketsHandler, uint32_t events)
{
	sockaddr_in		addr;
	socklen_t		addrLength;

	if (!(events & EPOLLIN))
		return ;
	addrLength = sizeof(addr);
	const int		fd = socketData.getFd();
	const uint32_t	newConnectionEvents = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
	const int 		newConnectionFd = accept(fd, (sockaddr *)&addr, &addrLength);

	if (checkError(newConnectionFd, -1, "accept() : ") == -1)
		return ;
	if (socketsHandler->addFdToListeners(newConnectionFd, processRequests, *socketsHandler, newConnectionEvents) == -1)
	{
		std::cerr << "Can't accept new connection" << std::endl;
		checkError(close(newConnectionFd), -1, "close() : ");
	}
	else
		std::cout << "Accepted a new connection, fd : " << newConnectionFd << std::endl;
}