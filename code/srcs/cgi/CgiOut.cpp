#include <sys/epoll.h>	// for EPOLLIN

#include "CgiOut.hpp"	// for CgiOut

CgiOut::CgiOut
(
	int fd,
	SocketsHandler &socketsHandler,
	const std::vector<ServerConfiguration> &serverConfigurations
) :
	AFdData(fd, socketsHandler, serverConfigurations)
{
}

CgiOut::~CgiOut()
{

}

void	CgiOut::callback(uint32_t events)
{
	if (!(events & EPOLLIN))
		return ;
}
