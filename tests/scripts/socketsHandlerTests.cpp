#include <iostream>
#include <sys/socket.h>
#include <strings.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <csignal>

#include "EPollHandler.hpp"
#include "Configuration.hpp"
#include "parsing.hpp"

#include "utils.cpp"

#define PATH_TO_TESTS "../tests/scripts/"

void	createAllServerSockets(const Configuration &conf, EPollHandler &ePollHandler);

void	tryCreatingMultipleInstance(const Configuration &conf)
{
	EPollHandler		ePollHandler(conf);

	printInfo("Try creating multiples instance");
	try
	{
		EPollHandler	ePollHandler2(conf);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
	try
	{
		EPollHandler	ePollHandler2(conf);
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
}

void	createAndDestroyMultipleInstance(const Configuration& conf)
{
	printInfo("try creating and destroying multiple instances");
	{
		EPollHandler	ePollHandler(conf);
	}
	{
		EPollHandler	ePollHandler(conf);
	}
	verify(true);
}

void	tryPassingAWrongIPV6Array(const Configuration& conf)
{
	(void)conf;
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

void	executeCallbackWithIndexTooBig(int errorFd, const Configuration& conf)
{
	EPollHandler		ePollHandler(conf);

	printInfo("Try executing callSocketCallback with an index too big");
	printInfo("Should output an error");
	ePollHandler.callSocketCallback(5);
	verify(checkError(errorFd));
}

void	bindUnixSocketWithWrongFd(int errorFd, const Configuration &conf)
{
	EPollHandler	ePollHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "test.sock";
	Host			host(path);

	printInfo("Try binding a wrong fd");
	printInfo("Should output an error");
	ePollHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketWithExistingFile(int errorFd, const Configuration& conf)
{
	EPollHandler	ePollHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "unixSocketsTests/test.txt";
	Host			host(path);

	printInfo("Try creating a unix socket with an existing file");
	printInfo("Should output an error");
	ePollHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketWithExistingDirectory(int errorFd, const Configuration& conf)
{
	EPollHandler	ePollHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "unixSocketsTests/";
	Host			host(path);

	printInfo("Try creating a unix socket with an existing directory");
	printInfo("Should output an error");
	ePollHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketInNoRightFolder(int errorFd, const Configuration& conf)
{
	EPollHandler	ePollHandler(conf);
	std::string		path(PATH_TO_TESTS);

	path += "unixSocketsTests/noRight/test.sock";
	Host			host(path);

	printInfo("Try creating a unix socket in a directory with no right");
	printInfo("Should output an error");
	ePollHandler.bindFdToHost(-1, host);
	verify(checkError(errorFd));
}

void	creatingUnixSocketWithExistingSocket(int errorFd, const Configuration& conf)
{
	EPollHandler		ePollHandler(conf);
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
	ePollHandler.bindFdToHost(secondSocket, host);
	verify(!checkError(errorFd));
	close(firstSocket);
	close(secondSocket);
	std::remove(path.c_str());
}

void	creatingUnixSocketWithNothing(int errorFd, const Configuration& conf)
{
	EPollHandler		ePollHandler(conf);
	std::string			path(PATH_TO_TESTS);
	path += "unixSocketsTests/test.sock";
	int					secondSocket;

	secondSocket = socket(AF_UNIX, SOCK_STREAM, 0);

	Host			host(path);

	printInfo("Try creating a unix with a path that points to nothing");
	printInfo("Shouldn't output an error");
	ePollHandler.bindFdToHost(secondSocket, host);
	verify(!checkError(errorFd));
	close(secondSocket);
	std::remove(path.c_str());
}

void	runTests(const Configuration& conf)
{
	int	tube[2];

	if (redirectSTDERR(tube) == false)
		return ;
	tryCreatingMultipleInstance(conf);
	createAndDestroyMultipleInstance(conf);
	tryPassingAWrongIPV6Array(conf);
	executeCallbackWithIndexTooBig(tube[0], conf);
	bindUnixSocketWithWrongFd(tube[0], conf);
	creatingUnixSocketWithExistingFile(tube[0], conf);
	creatingUnixSocketWithExistingDirectory(tube[0], conf);
	creatingUnixSocketInNoRightFolder(tube[0], conf);
	creatingUnixSocketWithExistingSocket(tube[0], conf);
	creatingUnixSocketWithNothing(tube[0], conf);
	close(tube[0]);
	close(tube[1]);
}

int	main(int argc, char **argv)
{
	if (argc == 1)
		std::cout << "No argument supplied, using the default path for the configuration." << std::endl;
	else if (argc > 2)
	{
		std::cout << "Too much arguments supplied: webserv (configuration/path)?" << std::endl;
		return (EXIT_FAILURE);
	}
	try
	{
		if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : "))
			return (EXIT_FAILURE);
		if (checkError(std::signal(SIGPIPE, SIG_IGN), SIG_ERR, "signal() : "))
			return (EXIT_FAILURE);

		Configuration	conf;
		std::string		file;

		if (argc == 2)
			readfile(argv[1], file);
		else
			readfile(DEFAULT_CONFIG_PATH, file);
		parseFile(conf, file);
		runTests(conf);
		return (getReturnCodeWithSignal());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
