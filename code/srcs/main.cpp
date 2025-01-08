#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>

#include "socketCommunication.hpp"

void	initializeConfiguration(Configuration &conf)
{
	Host								host(0, AF_INET, 8080);
	std::vector<ServerConfiguration>	serverConfigurations;

	conf.maxEvents = 500;
	conf.maxConnectionBySocket = 100;
	conf[host] = serverConfigurations;
	conf[host].push_back(ServerConfiguration());
	conf[host][0].host = &host;
}

/**
 * @brief Create a mostly uninitialize Configuration and call the function to
 * handle write and read request. It call the handleIoEvents till he receive a signal.
 * @throw This function may throw
 * @return Return the signal + 128
 */
int	main(void)
{
	Configuration	conf;

	initializeConfiguration(conf);
	while(getSignalStatus() == NO_SIGNAL)
	{
		try
		{
			handleIOEvents(conf);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << std::endl;
		}
	}
	return (getReturnCodeWithSignal());
}