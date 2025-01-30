#include <netinet/in.h>             // for sockaddr_in
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLET, EPOLLHUP, EPOLLIN
#include <sys/socket.h>             // for accept, recv, MSG_DONTWAIT, sockl...
#include <unistd.h>                 // for close
#include <iostream>                 // for basic_ostream, operator<<, endl
#include <string>                   // for char_traits, basic_string

#include "SocketData.hpp"           // for SocketData
#include "SocketsHandler.hpp"       // for SocketsHandle"
#include "socketCommunication.hpp"  // for acceptConnection

static void	processRequests(SocketData &socketData, SocketsHandler *socketsHandler, uint32_t events)
{
	(void)socketData;
	(void)socketsHandler;
	(void)events;
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
	try
	{
		SocketData * const socketData = new SocketData(newConnectionFd, *socketsHandler, processRequests);
		if (socketsHandler->addFdToListeners(*socketData, newConnectionEvents) == -1)
		{
			delete socketData;
			std::cerr << "Can't accept new connection" << std::endl;
			checkError(close(newConnectionFd), -1, "close() : ");
		}
		else
			std::cout << "Accepted a new connection, fd : " << newConnectionFd << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
}