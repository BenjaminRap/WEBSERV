#include <fcntl.h>                  // for fcntl, FD_CLOEXEC, F_SETFL, O_NON...
#include <netinet/in.h>             // for sockaddr_in
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLIN, EPOLLERR, EPOLLET, EPOLLHUP
#include <sys/socket.h>             // for accept, sockaddr, socklen_t
#include <unistd.h>                 // for close
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, operator<<, basic_...
#include <vector>                   // for vector

#include "AFdData.hpp"              // for AFdData
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "ServerSocketData.hpp"     // for ServerSocketData
#include "SocketsHandler.hpp"       // for SocketsHandler
#include "socketCommunication.hpp"  // for checkError

class ServerConfiguration;

//***********************Cosntructors / Destructors****************************/

ServerSocketData::ServerSocketData
(
	int fd ,
	SocketsHandler &socketsHandler,
	const std::vector<ServerConfiguration> &serverConfiguration
) :
	AFdData(fd, socketsHandler, serverConfiguration)
{

}

ServerSocketData::~ServerSocketData(void)
{
	std::cout << "ServerSocketData removed !" << std::endl;
}


//**************************Member functions***********************************/

/**
 * @brief Accept a connection request, create a new fd, add it to the epoll interest
 * list and add a ConnectedSocketData node to the socketsHandler list.
 * @param events The events registered by epoll.
 */
void	ServerSocketData::acceptConnection(uint32_t events)
{
	sockaddr_in		addr;
	socklen_t		addrLength;

	if (!(events & EPOLLIN))
		return ;
	addrLength = sizeof(addr);
	const uint32_t	newConnectionEvents = EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
	const int 		newConnectionFd = accept(_fd, (sockaddr *)&addr, &addrLength);

	if (checkError(newConnectionFd, -1, "accept() : "))
		return ;
	if (addFlagsToFd(newConnectionFd, O_NONBLOCK | FD_CLOEXEC) == -1)
	{
		checkError(close(newConnectionFd), -1, "close() : ");
		return ;
	}
	try
	{
		ConnectedSocketData& connectedSocketData = *(new ConnectedSocketData(newConnectionFd, _socketsHandler, _serverConfigurations));
		if (_socketsHandler.addFdToListeners(connectedSocketData, newConnectionEvents) == -1)
		{
			delete &connectedSocketData;
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

void	ServerSocketData::callback(uint32_t events)
{
	acceptConnection(events);
}
