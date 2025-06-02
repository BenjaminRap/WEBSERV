#include <stdint.h>                 // for uint32_t
#include <sys/epoll.h>              // for EPOLLERR, EPOLLIN
#include <sys/socket.h>             // for accept, sockaddr, socklen_t
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, char_traits, opera...
#include <vector>                   // for vector

#include "AEPollFd.hpp"             // for AEPollFd
#include "ConnectedSocketData.hpp"  // for ConnectedSocketData
#include "EPollHandler.hpp"         // for EPollHandler
#include "Host.hpp"                 // for sockaddr_in_u, Host (ptr only)
#include "ServerSocketData.hpp"     // for ServerSocketData, SERVER_EVENTS
#include "socketCommunication.hpp"  // for checkError, closeFdAndPrintError

class ServerConfiguration;  // lines 17-17

//***********************Cosntructors / Destructors****************************/

ServerSocketData::ServerSocketData
(
	int fd ,
	EPollHandler &ePollHandler,
	const std::vector<ServerConfiguration> &serverConfiguration,
	const Host& host
) :
	AEPollFd(fd, ePollHandler, SERVER_SOCKET_DATA, SERVER_EVENTS),
	_host(host),
	_serverConfigurations(serverConfiguration)
{
}

ServerSocketData::~ServerSocketData(void)
{
}


//**************************Member functions***********************************/

void	ServerSocketData::acceptConnection(uint32_t events)
{
	if (!(events & EPOLLIN))
		return ;
	sockaddr_in_u	addr;
	socklen_t		addrLength;

	addrLength = sizeof(addr);
	int 			newFd = accept(getFd(), (sockaddr *)&addr, &addrLength);

	if (checkError(newFd, -1, "accept() : "))
		return ;
	try
	{
		ConnectedSocketData* connectedSocketData = new ConnectedSocketData(newFd, _ePollHandler, _serverConfigurations, _host, addr);
		if (!_ePollHandler.addFdToList(*connectedSocketData))
		{
			std::cerr << "Can't accept new connection" << std::endl;
			delete connectedSocketData;
			newFd = -1;
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		closeFdAndPrintError(newFd);
	}
}

void	ServerSocketData::callback(uint32_t events)
{
	if (!getIsActive())
		return ;
	if (events & EPOLLERR)
		removeFromEPollHandler();
	else
		acceptConnection(events);
}
