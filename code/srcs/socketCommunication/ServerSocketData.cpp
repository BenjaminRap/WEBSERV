#include <fcntl.h>                  // for fcntl, FD_CLOEXEC, F_SETFL, O_NON...
#include <netinet/in.h>             // for sockaddr_in
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLIN, EPOLLERR, EPOLLET, EPOLLHUP
#include <sys/socket.h>             // for accept, sockaddr, socklen_t
#include <unistd.h>                 // for close
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, operator<<, basic_...
#include <vector>                   // for vector

#include "ASocketData.hpp"          // for ASocketData
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "ServerSocketData.hpp"     // for ServerSocketData
#include "EPollHandler.hpp"         // for EPollHandler
#include "socketCommunication.hpp"  // for checkError

class ServerConfiguration;

//***********************Cosntructors / Destructors****************************/

ServerSocketData::ServerSocketData
(
	int fd ,
	EPollHandler &ePollHandler,
	const std::vector<ServerConfiguration> &serverConfiguration
) :
	ASocketData(fd, ePollHandler, serverConfiguration, SERVER_SOCKET_DATA)
{

}

ServerSocketData::~ServerSocketData(void)
{
	std::cout << "ServerSocketData removed !" << std::endl;
}


//**************************Member functions***********************************/

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
		closeFdAndPrintError(newConnectionFd);
		return ;
	}
	try
	{
		ConnectedSocketData& connectedSocketData = *(new ConnectedSocketData(newConnectionFd, *_ePollHandler, _serverConfigurations));
		if (_ePollHandler->addFdToList(connectedSocketData, newConnectionEvents) == -1)
		{
			std::cerr << "Can't accept new connection" << std::endl;
			delete &connectedSocketData;
			closeFdAndPrintError(newConnectionFd);
		}
		else
			std::cout << "Accepted a new connection, fd : " << newConnectionFd << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		closeFdAndPrintError(newConnectionFd);
	}
}

void	ServerSocketData::callback(uint32_t events)
{
	if (events & (EPOLLHUP | EPOLLRDHUP | EPOLLERR))
		removeFromEPollHandler();
	else
		acceptConnection(events);
}
