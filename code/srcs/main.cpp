#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

#include "socketCommunication.hpp"

int	main(void)
{
	Configuration		conf;
	ServerConfiguration	serverConf;

	conf.maxEvents = 500;
	conf.maxConnectionBySocket = 100;
	serverConf.host.address = 0;
	serverConf.host.port = 8080;
	serverConf.host.family = AF_INET;
	conf.push_back(serverConf);
	while(getSignalStatus() == NO_SIGNAL)
	{
		handleIOEvents(conf);
	}
	return (getReturnCodeWithSignal());
}