#include <iostream>
#include <list>
#include <cstdlib>

#include "ServerSocketData.hpp"
#include "SocketsHandler.hpp"
#include "Configuration.hpp"

#include "utils.cpp"

void	checkInvalidArgument(int minFd, SocketsHandler &socketsHandler)
{
	printInfo("Try creating ServerSocketData with invalid socket");

	for (int i = minFd - 1; i > -3; i--)
	{
		try
		{
			ServerSocketData	serverSocketData(i, socketsHandler);
			verify(false);
		}
		catch(const std::exception& e)
		{
			std::cout << e.what() << '\n';
			verify(true);
		}
	}
	
}

void	tryGettingUnsetIterator(SocketsHandler &socketsHandler)
{

	ServerSocketData	serverSocketData(4, socketsHandler);

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

void	TrySettingWrongIterator(int errorFd, SocketsHandler &socketsHandler)
{
	std::list<FdData *>	socketsData;

	printInfo("Try setting the wrong iterator");
	socketsData.push_front(new ServerSocketData(4, socketsHandler));
	socketsData.push_front(new ServerSocketData(5, socketsHandler));
	printInfo("Should output an error message :");
	socketsData.back()->setIterator(socketsData.begin());
	for (std::list<FdData *>::iterator it = socketsData.begin(); it != socketsData.end(); it++)
		delete *it;
	verify(checkError(errorFd));
}

void	trySettingIteratorTwice(int errorFd, SocketsHandler &socketsHandler)
{
	std::list<FdData *>	socketsData;

	printInfo("try setting an iterator twice");
	socketsData.push_front(new ServerSocketData(4, socketsHandler));
	printInfo("Should output an error message :");
	socketsData.front()->setIterator(socketsData.begin());
	socketsData.front()->setIterator(socketsData.begin());
	for (std::list<FdData *>::iterator it = socketsData.begin(); it != socketsData.end(); it++)
		delete *it;
	verify(checkError(errorFd));
}

void	tryUsingIterator(SocketsHandler &socketsHandler)
{
	std::list<FdData *>	socketsData;

	printInfo("try using an iterator");
	socketsData.push_front(new ServerSocketData(4, socketsHandler));
	socketsData.front()->setIterator(socketsData.begin());
	for (std::list<FdData *>::iterator it = socketsData.begin(); it != socketsData.end(); )
	{
		std::list<FdData *>::iterator storedIt = (*it)->getIterator();
		delete *it;
		it++;
		socketsData.erase(storedIt);
	}
	verify(socketsData.size() == 0);
}

int	main(void)
{
	int	tube[2];
	Configuration	conf;
	SocketsHandler	socketsHandler(conf);

	if (redirectSTDERR(tube) == false)
		return (EXIT_FAILURE);
	checkInvalidArgument(4, socketsHandler);
	tryGettingUnsetIterator(socketsHandler);
	TrySettingWrongIterator(tube[0], socketsHandler);
	trySettingIteratorTwice(tube[0], socketsHandler);
	tryUsingIterator(socketsHandler);
	close(tube[0]);
	close(tube[1]);
}