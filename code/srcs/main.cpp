#include <iostream>
#include <string.h>
#include <errno.h>

#include "Configuration.hpp"
#include "socketCommunication.hpp"

int	main(void)
{
	Configuration		conf;
	ServerConfiguration	serverConf;
	int					ret;
	int					previousRet;

	conf.maxEvents = 500;
	conf.maxConnectionBySocket = 100;
	serverConf.host = 0;
	serverConf.port = 8080;
	conf.push_back(serverConf);
	ret = 1;
	while(ret != 0)
	{
		ret = handleIOEvents(conf);
		if (ret == -1 && previousRet != -1)
			strerror(errno);
		previousRet = ret;
	}
}