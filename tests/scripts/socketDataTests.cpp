#include <iostream>
#include <list>

#include "SocketData.hpp"

#include "utils.cpp"

void	checkInvalidArgument(int minFd)
{
	printInfo("Try creating SocketData with invalid socket");

	for (int i = minFd - 1; i > -3; i--)
	{
		try
		{
			SocketData	socketData(i, (void *)NULL, callback);
			verify(false);
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
			verify(true);
		}
	}
	
}

void	tryGettingUnsetIterator()
{
	SocketData	socketData(4, (void *)NULL, callback);

	printInfo("Try getting an unset iterator");
	try
	{
		socketData.getIterator();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		verify(true);
	}
}

void	TrySettingWrongIterator()
{
	std::list<SocketData>	socketsData;

	printInfo("Try setting the wrong iterator");
	socketsData.push_front(SocketData(4, (void *)NULL, callback));
	socketsData.push_front(SocketData(5, (void *)NULL, callback));
	printInfo("Should output an error message :");
	socketsData.back().setIterator(socketsData.begin());
}

void	trySettingIteratorTwice()
{
	std::list<SocketData>	socketsData;

	printInfo("try setting an iterator twice");
	socketsData.push_front(SocketData(4, (void *)NULL, callback));
	printInfo("Should output an error message :");
	socketsData.front().setIterator(socketsData.begin());
	socketsData.front().setIterator(socketsData.begin());
}

void	tryUsingIterator()
{
	std::list<SocketData>	socketsData;

	printInfo("try using an iterator");
	socketsData.push_front(SocketData(4, (void *)NULL, callback));
	socketsData.front().setIterator(socketsData.begin());
	socketsData.erase(socketsData.front().getIterator());
	verify(socketsData.size() == 0);
}

int	main(void)
{
	checkInvalidArgument(4);
	tryGettingUnsetIterator();
	TrySettingWrongIterator();
	trySettingIteratorTwice();
	tryUsingIterator();
}