#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

#include "socketCommunication.hpp"

int	main(void)
{
	Configuration						conf;
	Host								host(0, AF_INET, 8080);
	std::vector<ServerConfiguration>	serverConfigurations;

	conf.maxEvents = 500;
	conf.maxConnectionBySocket = 100;
	conf[host] = serverConfigurations;
	conf[host].push_back(ServerConfiguration());
	conf[host][0].host = &host;
	while(getSignalStatus() == NO_SIGNAL)
	{
		handleIOEvents(conf);
	}
	return (getReturnCodeWithSignal());
}