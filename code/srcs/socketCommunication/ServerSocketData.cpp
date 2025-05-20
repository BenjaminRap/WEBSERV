#include <netinet/in.h>             // for sockaddr_in
#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLIN
#include <sys/socket.h>             // for accept, sockaddr, socklen_t
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, char_traits, opera...
#include <vector>                   // for vector

#include "AFdData.hpp"              // for AFdDataChilds
#include "ASocketData.hpp"          // for ASocketData
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "EPollHandler.hpp"         // for EPollHandler
#include "ServerSocketData.hpp"     // for ServerSocketData, SERVER_EVENTS
#include "socketCommunication.hpp"  // for closeFdAndPrintError, checkError

class ServerConfiguration;  // lines 17-17

//***********************Cosntructors / Destructors****************************/

ServerSocketData::ServerSocketData
(
	int fd ,
	EPollHandler &ePollHandler,
	const std::vector<ServerConfiguration> &serverConfiguration,
	const Host& host
) :
	ASocketData(fd, ePollHandler, serverConfiguration, SERVER_SOCKET_DATA, SERVER_EVENTS),
	_host(host)
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
	const int 		newFd = accept(_fd, (sockaddr *)&addr, &addrLength);

	if (checkError(newFd, -1, "accept() : "))
		return ;
	try
	{
		ConnectedSocketData* connectedSocketData = new ConnectedSocketData(newFd, *_ePollHandler, _serverConfigurations, _host, addr);
		if (_ePollHandler->addFdToList(*connectedSocketData) == -1)
		{
			std::cerr << "Can't accept new connection" << std::endl;
			delete connectedSocketData;
			closeFdAndPrintError(newFd);
		}
		else
			std::cout << "Accepted a new connection, fd : " << newFd << std::endl;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		closeFdAndPrintError(newFd);
	}
}

void	ServerSocketData::callback(uint32_t events)
{
	if (events & EPOLLERR)
		removeFromEPollHandler();
	else
		acceptConnection(events);
}
