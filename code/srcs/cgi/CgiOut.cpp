#include "CgiOut.hpp"

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
	(void)events;
}
