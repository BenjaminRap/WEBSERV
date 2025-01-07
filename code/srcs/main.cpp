#include <iostream>
#include <string.h>
#include <errno.h>

#include "socketCommunication.hpp"

int	main(void)
{
	Configuration		conf;
	ServerConfiguration	serverConf;

	conf.maxEvents = 500;
	conf.maxConnectionBySocket = 100;
	serverConf.host = 0;
	serverConf.port = 8080;
	conf.push_back(serverConf);
	while(getSignalStatus() == NO_SIGNAL)
	{
		handleIOEvents(conf);
	}
	return (128 + getSignalStatus());
}