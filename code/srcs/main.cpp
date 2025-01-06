#include <iostream>

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
	handleIOEvents(conf);
}