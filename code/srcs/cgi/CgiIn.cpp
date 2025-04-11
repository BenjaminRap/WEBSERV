#include "CgiIn.hpp"

CgiIn::CgiIn
(
	int fd,
	SocketsHandler &socketsHandler,
	const std::vector<ServerConfiguration> &serverConfigurations
) :
	AFdData(fd, socketsHandler, serverConfigurations)
{
}

CgiIn::~CgiIn()
{

}

void	CgiIn::callback(uint32_t events)
{
	(void)events;
}
