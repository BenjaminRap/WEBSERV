#include <iostream>
#include <csignal>
#include <vector>
#include <list>
#include <cstdlib>

#include "ServerSocketData.hpp"
#include "SocketsHandler.hpp"
#include "Configuration.hpp"
#include "parsing.hpp"

#include "utils.cpp"

void	checkInvalidArgument(int minFd, SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> serverConfs)
{
	printInfo("Try creating ServerSocketData with invalid socket");

	for (int i = minFd - 1; i > -3; i--)
	{
		try
		{
			ServerSocketData	serverSocketData(i, socketsHandler, serverConfs);
			verify(false);
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << '\n';
			verify(true);
		}
	}
	
}

void	tryGettingUnsetIterator(SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> serverConfs)
{

	ServerSocketData	serverSocketData(4, socketsHandler, serverConfs);

	printInfo("Try getting an unset iterator");
	try
	{
		serverSocketData.getIterator();
	}
	catch(const std::exception& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
}

void	TrySettingWrongIterator(int errorFd, SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> serverConfs)
{
	std::list<AFdData *>	socketsData;

	printInfo("Try setting the wrong iterator");
	socketsData.push_front(new ServerSocketData(4, socketsHandler, serverConfs));
	socketsData.push_front(new ServerSocketData(5, socketsHandler, serverConfs));
	printInfo("Should output an error message :");
	socketsData.back()->setIterator(socketsData.begin());
	verify(checkError(errorFd));
	for (std::list<AFdData *>::iterator it = socketsData.begin(); it != socketsData.end(); it++)
		delete *it;
}

void	trySettingIteratorTwice(int errorFd, SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> serverConfs)
{
	std::list<AFdData *>	socketsData;

	printInfo("try setting an iterator twice");
	socketsData.push_front(new ServerSocketData(4, socketsHandler, serverConfs));
	printInfo("Should output an error message :");
	socketsData.front()->setIterator(socketsData.begin());
	socketsData.front()->setIterator(socketsData.begin());
	verify(checkError(errorFd));
	for (std::list<AFdData *>::iterator it = socketsData.begin(); it != socketsData.end(); it++)
		delete *it;
}

void	tryUsingIterator(SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> serverConfs)
{
	std::list<AFdData *>	socketsData;

	printInfo("try using an iterator");
	socketsData.push_front(new ServerSocketData(4, socketsHandler, serverConfs));
	socketsData.front()->setIterator(socketsData.begin());
	for (std::list<AFdData *>::iterator it = socketsData.begin(); it != socketsData.end(); )
	{
		std::list<AFdData *>::iterator storedIt = (*it)->getIterator();
		delete *it;
		it++;
		socketsData.erase(storedIt);
	}
	verify(socketsData.size() == 0);
}

void	runTests(const Configuration& conf)
{
	int										tube[2];
	SocketsHandler							socketsHandler(conf);
	const std::vector<ServerConfiguration>&	serverConfs = conf.begin()->second;

	if (redirectSTDERR(tube) == false)
		return ;
	checkInvalidArgument(4, socketsHandler, serverConfs);
	tryGettingUnsetIterator(socketsHandler, serverConfs);
	TrySettingWrongIterator(tube[0], socketsHandler, serverConfs);
	trySettingIteratorTwice(tube[0], socketsHandler, serverConfs);
	tryUsingIterator(socketsHandler, serverConfs);
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
