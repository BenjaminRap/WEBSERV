#include <netinet/in.h>
#include <sys/epoll.h>
#include <unistd.h>

#include "ServerSocketData.hpp"
#include "socketCommunication.hpp"
#include "SocketsHandler.hpp"
#include "ConnectedSocketData.hpp"

//***********************Cosntructors / Destructors****************************/

ServerSocketData::ServerSocketData(int fd , SocketsHandler &socketsHandler) :
	FdData(fd, socketsHandler)
{

}

ServerSocketData::~ServerSocketData(void)
{

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
	const uint32_t	newConnectionEvents = EPOLLIN | EPOLLET | EPOLLRDHUP | EPOLLHUP | EPOLLERR;
	const int 		newConnectionFd = accept(_fd, (sockaddr *)&addr, &addrLength);

	if (checkError(newConnectionFd, -1, "accept() : ") == -1)
		return ;
	try
	{
		ConnectedSocketData * const connectedSocketData = new ConnectedSocketData(newConnectionFd, _socketsHandler);
		if (_socketsHandler.addFdToListeners(connectedSocketData, newConnectionEvents) == -1)
		{
			delete connectedSocketData;
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