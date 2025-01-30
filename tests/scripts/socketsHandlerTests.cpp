#include <iostream>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>

#include "SocketsHandler.hpp"
#include "Configuration.hpp"

#include "utils.cpp"

#define PATH_TO_TESTS "../tests/scripts/"

void	createAllServerSockets(const Configuration &conf, SocketsHandler &socketsHandler);

void	tryCreatingMultipleInstance()
{
	const Configuration	conf;
	SocketsHandler		socketsHandler(conf);

	printInfo("Try creating multiples instance");
	try
	{
		SocketsHandler	socketsHandler2(conf);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
	try
	{
		SocketsHandler	socketsHandler2(conf);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
}

void	createAndDestroyMultipleInstance()
{
	const Configuration	conf;

	printInfo("try creating and destroying multiple instances");
	{
		SocketsHandler	socketsHandler(conf);
	}
	{
		SocketsHandler	socketsHandler(conf);
	}
	verify(true);
}

void	addServerConfiguration(Host host, Configuration &conf)
{

	std::vector<ServerConfiguration>	serverConfigurations;
	conf[host] = serverConfigurations;
	conf[host].push_back(ServerConfiguration());
}

void	bindTwiceSameHost(int errorFd)
{
	Configuration	conf;

	printInfo("Try listening twice to the same host");
	addServerConfiguration(Host((in_addr_t)0, (in_port_t)8080), conf);
	addServerConfiguration(Host((in_addr_t)0, (in_port_t)8080), conf);

	SocketsHandler						socketsHandler(conf);

	std::cout << "\033[0;35m" << "Shouldn't output an error message, because map has unique key, map size = " << conf.size() << "\033[0m\n" << std::endl;
	createAllServerSockets(conf, socketsHandler);
	verify(!checkError(errorFd));
}

void	bindTwiceSameHostWithDiffIpFamily(int errorFd)
{
	Configuration						conf;
	uint8_t								addr[16];

	std::memset((char *)addr, '\0', sizeof(addr));
	printInfo("Try listening twice to the same host with different ip family");
	addServerConfiguration(Host((in_addr_t)0, (in_port_t)8080), conf);
	addServerConfiguration(Host(addr, (in_port_t)8080), conf);


	SocketsHandler						socketsHandler(conf);

	printInfo("Shouldn't output an error message if IPV6Only is set");
	createAllServerSockets(conf, socketsHandler);
	verify(!checkError(errorFd));
}

void	tryPassingAWrongIPV6Array()
{
	// uint8_t								*addr2;
	// uint8_t								addr3[8];
	// uint8_t								addr4[32];
	// Configuration						conf;

	printInfo("Try passing a wrong ipv6 array (doesn't compile, see test main)");
	// addServerConfiguration(Host(addr2, (in_port_t)8080), conf);
	// addServerConfiguration(Host(addr3, (in_port_t)8080), conf);
	// addServerConfiguration(Host(addr4, (in_port_t)8080), conf);
	verify(true);
}

void	addWrongFdToListeners(int errorFd)
{
	const Configuration	conf;
	SocketsHandler		socketsHandler(conf);
	int					data;

	data = 0;
	printInfo("Try passing an fd to listeners that is inferior to 4");
	printInfo("Should output an error");
	try
	{
		new SocketData(1, data, callback);
		verify(false);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		verify(true);
	}
	printInfo("Try passing an fd to listeners that isn't open");
	printInfo("Should output an error");
	socketsHandler.addFdToListeners(*(new SocketData(10, data, callback)), EPOLLIN);
	verify(checkError(errorFd));
}

void	executeCallbackWithIndexTooBig(int errorFd)
{
	const Configuration	conf;
	SocketsHandler		socketsHandler(conf);

	printInfo("Try executing callSocketCallback with an index too big");
	printInfo("Should output an error");
	socketsHandler.callSocketCallback(5);
	verify(checkError(errorFd));
}

void	bindUnixSocketWithWrongFd(int errorFd)
{
	Configuration	conf;
	SocketsHandler	socketsHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "test.sock";
	Host			host(path);

	printInfo("Try binding a wrong fd");
	printInfo("Should output an error");
	socketsHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketWithExistingFile(int errorFd)
{
	Configuration	conf;
	SocketsHandler	socketsHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "unixSocketsTests/test.txt";
	Host			host(path);

	printInfo("Try creating a unix socket with an existing file");
	printInfo("Should output an error");
	socketsHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketWithExistingDirectory(int errorFd)
{
	Configuration	conf;
	SocketsHandler	socketsHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "unixSocketsTests/";
	Host			host(path);

	printInfo("Try creating a unix socket with an existing directory");
	printInfo("Should output an error");
	socketsHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketInNoRightFolder(int errorFd)
{
	Configuration	conf;
	SocketsHandler	socketsHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "unixSocketsTests/noRight/test.sock";
	Host			host(path);

	printInfo("Try creating a unix socket in a directory with no right");
	printInfo("Should output an error");
	socketsHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketWithExistingSocket(int errorFd)
{
	Configuration		conf;
	SocketsHandler		socketsHandler(conf);
	std::string			path(PATH_TO_TESTS);
	path += "unixSocketsTests/test.sock";
	int					firstSocket;
	int					secondSocket;
	struct sockaddr_un	addr;

	firstSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	secondSocket = socket(AF_UNIX, SOCK_STREAM, 0);
	addr.sun_family = AF_UNIX;
	std::memcpy(addr.sun_path, path.c_str(), path.size() + 1);
	bind(firstSocket, (const sockaddr *)&addr, sizeof(sockaddr_un));

	Host			host(path);

	printInfo("Try creating a unix with an existing socket");
	printInfo("Shouldn't output an error, even though the file is deleted, previous connection to the first socket aren't closed, the first socket close when there is no more connection");
	socketsHandler.bindFdToHost(secondSocket, host);
	verify(!checkError(errorFd));
	close(firstSocket);
	close(secondSocket);
	std::remove(path.c_str());
}

void	creatingUnixSocketWithNothing(int errorFd)
{
	Configuration		conf;
	SocketsHandler		socketsHandler(conf);
	std::string			path(PATH_TO_TESTS);
	path += "unixSocketsTests/test.sock";
	int					secondSocket;

	secondSocket = socket(AF_UNIX, SOCK_STREAM, 0);

	Host			host(path);

	printInfo("Try creating a unix with a path that points to nothing");
	printInfo("Shouldn't output an error");
	socketsHandler.bindFdToHost(secondSocket, host);
	verify(!checkError(errorFd));
	close(secondSocket);
	std::remove(path.c_str());
}

int	main(void)
{
	int	tube[2];

	if (redirectSTDERR(tube) == false)
		return (EXIT_FAILURE);
	tryCreatingMultipleInstance();
	createAndDestroyMultipleInstance();
	bindTwiceSameHost(tube[0]);
	bindTwiceSameHostWithDiffIpFamily(tube[0]);
	tryPassingAWrongIPV6Array();
	addWrongFdToListeners(tube[0]);
	executeCallbackWithIndexTooBig(tube[0]);
	bindUnixSocketWithWrongFd(tube[0]);
	creatingUnixSocketWithExistingFile(tube[0]);
	creatingUnixSocketWithExistingDirectory(tube[0]);
	creatingUnixSocketInNoRightFolder(tube[0]);
	creatingUnixSocketWithExistingSocket(tube[0]);
	creatingUnixSocketWithNothing(tube[0]);
	close(tube[0]);
	close(tube[1]);
}