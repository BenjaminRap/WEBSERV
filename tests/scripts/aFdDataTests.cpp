#include <iostream>
#include <csignal>
#include <vector>
#include <list>
#include <cstdlib>

#include "ServerSocketData.hpp"
#include "EPollHandler.hpp"
#include "Configuration.hpp"
#include "parsing.hpp"

#include "utils.cpp"

void	checkInvalidArgument(int minFd, EPollHandler &ePollHandler, const std::vector<ServerConfiguration> serverConfs)
{
	printInfo("Try creating ServerSocketData with invalid socket");

	for (int i = minFd - 1; i > -3; i--)
	{
		try
		{
			ServerSocketData	serverSocketData(i, ePollHandler, serverConfs);
			verify(false);
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << '\n';
			verify(true);
		}
	}
}

void	tryGettingUnsetIterator(EPollHandler &ePollHandler, const std::vector<ServerConfiguration> serverConfs)
{

	ServerSocketData	serverSocketData(100, ePollHandler, serverConfs);

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

void	TrySettingWrongIterator(int errorFd, EPollHandler &ePollHandler, const std::vector<ServerConfiguration> serverConfs)
{
	std::list<AFdData *>	socketsData;

	printInfo("Try setting the wrong iterator");
	socketsData.push_front(new ServerSocketData(100, ePollHandler, serverConfs));
	socketsData.push_front(new ServerSocketData(200, ePollHandler, serverConfs));
	printInfo("Should output an error message :");
	ignoreSTDERR(errorFd);
	socketsData.back()->setIterator(socketsData.begin());
	verify(checkError(errorFd));
	for (std::list<AFdData *>::iterator it = socketsData.begin(); it != socketsData.end(); it++)
		delete *it;
}

void	trySettingIteratorTwice(int errorFd, EPollHandler &ePollHandler, const std::vector<ServerConfiguration> serverConfs)
{
	std::list<ASocketData *>	socketsData;

	printInfo("try setting an iterator twice");
	socketsData.push_front(new ServerSocketData(100, ePollHandler, serverConfs));
	printInfo("Should output an error message :");
	ignoreSTDERR(errorFd);
	socketsData.front()->setIterator(socketsData.begin());
	socketsData.front()->setIterator(socketsData.begin());
	verify(checkError(errorFd));
	for (std::list<AFdData *>::iterator it = socketsData.begin(); it != socketsData.end(); it++)
		delete *it;
}

void	tryUsingIterator(EPollHandler &ePollHandler, const std::vector<ServerConfiguration> serverConfs)
{
	std::list<ASocketData *>	socketsData;

	printInfo("try using an iterator");
	socketsData.push_front(new ServerSocketData(100, ePollHandler, serverConfs));
	socketsData.front()->setIterator(socketsData.begin());
	for (std::list<ASocketData *>::iterator it = socketsData.begin(); it != socketsData.end(); )
	{
		std::list<ASocketData *>::iterator storedIt = (*it)->getIterator();
		delete *it;
		it++;
		socketsData.erase(storedIt);
	}
	verify(socketsData.size() == 0);
}

void	runTests(const Configuration& conf)
{
	int										tube[2];
	EPollHandler							ePollHandler(conf);
	const std::vector<ServerConfiguration>&	serverConfs = conf.begin()->second;

	if (redirectSTDERR(tube) == false)
		return ;
	tryGettingUnsetIterator(ePollHandler, serverConfs);
	TrySettingWrongIterator(tube[0], ePollHandler, serverConfs);
	trySettingIteratorTwice(tube[0], ePollHandler, serverConfs);
	tryUsingIterator(ePollHandler, serverConfs);
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
