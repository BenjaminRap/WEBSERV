#include <iostream>
#include <sys/socket.h>

#include "SocketsHandler.hpp"
#include "Configuration.hpp"

void	createAllServerSockets(const Configuration &conf, SocketsHandler &socketsHandler);

void	verify(bool test)
{
	if (test)
		std::cout << "\033[0;32mOK\033[0m\n";
	else
		std::cout << "\033[0;31mKO\033[0m\n";
}

void	printInfo(std::string infos)
{
	std::cout << "\033[0;35m" << infos << "\033[0m\n" << std::endl;
}

void	tryCreatingMultipleInstance()
{
	SocketsHandler	socketsHandler(100);

	printInfo("Try creating multiples instance");
	try
	{
		SocketsHandler	socketsHandler2(100);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		verify(true);
	}
	try
	{
		SocketsHandler	socketsHandler2(100);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		verify(true);
	}
}

void	createAndDestroyMultipleInstance()
{
	printInfo("try creating and destroying multiple instances");
	{
		SocketsHandler	socketsHandler(100);
	}
	{
		SocketsHandler	socketsHandler(100);
	}
	verify(true);
}

void	addServerConfiguration(uint32_t address, sa_family_t family, uint16_t port, Configuration &conf)
{

	Host								host(address, family, port);
	std::vector<ServerConfiguration>	serverConfigurations;
	conf[host] = serverConfigurations;
	conf[host].push_back(ServerConfiguration());
	conf[host].back().host = &host;
}

void	InitializeConf(Configuration &conf)
{
	conf.maxEvents = 500;
	conf.maxConnectionBySocket = 100;
	conf.reuseAddr = true;
}

void	bindTwiceSameHost()
{
	Configuration						conf;

	printInfo("Try listening twice to the same host");
	InitializeConf(conf);
	addServerConfiguration(0, AF_INET, 8080, conf);
	addServerConfiguration(0, AF_INET, 8080, conf);

	SocketsHandler						socketsHandler(conf.maxEvents);

	std::cout << "\033[0;35m" << "Shouldn't output an error message, because map has unique key, map size = " << conf.size() << "\033[0m\n" << std::endl;
	createAllServerSockets(conf, socketsHandler);
	verify(true);
}

void	bindTwiceSameHostWithDiffIpFamily()
{
	Configuration						conf;

	printInfo("Try listening twice to the same host with different if family");
	InitializeConf(conf);
	addServerConfiguration(0, AF_INET, 8080, conf);
	addServerConfiguration(0, AF_INET6, 8080, conf);

	SocketsHandler						socketsHandler(conf.maxEvents);

	std::cout << "\033[0;35m" << "Shouldn't output an error message, because map has unique key, map size = " << conf.size() << "\033[0m\n" << std::endl;
	createAllServerSockets(conf, socketsHandler);
	verify(true);
}

int	main(void)
{
	tryCreatingMultipleInstance();
	createAndDestroyMultipleInstance();
	bindTwiceSameHost();
	bindTwiceSameHostWithDiffIpFamily();
}