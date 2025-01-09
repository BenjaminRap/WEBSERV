#include <iostream>
#include <sys/socket.h>

#include "SocketsHandler.hpp"
#include "Configuration.hpp"

void	createAllServerSockets(const Configuration &conf, SocketsHandler &socketsHandler);
void	bzero(char *value, size_t size);

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

void	callback(int fd, void *data)
{
	(void)fd;
	(void)data;
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

void	addServerConfiguration(Host host, Configuration &conf)
{

	std::vector<ServerConfiguration>	serverConfigurations;
	conf[host] = serverConfigurations;
	conf[host].push_back(ServerConfiguration());
}

void	bindTwiceSameHost()
{
	Configuration						conf;

	printInfo("Try listening twice to the same host");
	addServerConfiguration(Host((in_addr_t)0, (in_port_t)8080), conf);
	addServerConfiguration(Host((in_addr_t)0, (in_port_t)8080), conf);

	SocketsHandler						socketsHandler(conf.getMaxEvents());

	std::cout << "\033[0;35m" << "Shouldn't output an error message, because map has unique key, map size = " << conf.size() << "\033[0m\n" << std::endl;
	createAllServerSockets(conf, socketsHandler);
	verify(true);
}

void	bindTwiceSameHostWithDiffIpFamily()
{
	Configuration						conf;
	uint8_t								addr[16];


	bzero((char *)addr, sizeof(addr));
	printInfo("Try listening twice to the same host with different ip family");
	addServerConfiguration(Host((in_addr_t)0, (in_port_t)8080), conf);
	addServerConfiguration(Host(addr, (in_port_t)8080), conf);


	SocketsHandler						socketsHandler(conf.getMaxEvents());

	printInfo("Shouldn't output an error message if IPV6Only is set");
	createAllServerSockets(conf, socketsHandler);
	verify(true);
}

void	tryPassingAWrongIPV6Array()
{
	// uint8_t								*addr2;
	// uint8_t								addr3[8];
	// uint8_t								addr4[32];
	// Configuration						conf;

	std::cout << "Try passing a wrong ipv6 array (doesn't compile, see test main)" << std::endl;
	// addServerConfiguration(Host(addr2, (in_port_t)8080), conf);
	// addServerConfiguration(Host(addr3, (in_port_t)8080), conf);
	// addServerConfiguration(Host(addr4, (in_port_t)8080), conf);
	verify(true);
}

void	addWrongFdToListeners()
{
	Configuration	conf;
	SocketsHandler	socketsHandler(conf.getMaxEvents());

	printInfo("Try passing an fd to listeners that is inferior to 4");
	printInfo("Should output an error");
	socketsHandler.addFdToListeners(1, callback, NULL, EPOLLIN);
	verify(true);
	printInfo("Try passing an fd to listeners that isn't open");
	printInfo("Should output an error");
	socketsHandler.addFdToListeners(10, callback, NULL, EPOLLIN);
	verify(true);
}

void	ExecuteCallbackWithIndexTooBig()
{
	Configuration	conf;
	SocketsHandler	socketsHandler(conf.getMaxEvents());

	printInfo("Try executing callSocketCallback with an index too big");
	printInfo("Should output an error");
	socketsHandler.callSocketCallback(5);
	verify(true);
}

int	main(void)
{
	tryCreatingMultipleInstance();
	createAndDestroyMultipleInstance();
	bindTwiceSameHost();
	bindTwiceSameHostWithDiffIpFamily();
	tryPassingAWrongIPV6Array();
	addWrongFdToListeners();
	ExecuteCallbackWithIndexTooBig();
}