#include <iostream>
#include <string.h>
#include <errno.h>

#include "Configuration.hpp"
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
	if (handleIOEvents(conf) == -1)
		strerror(errno);
}