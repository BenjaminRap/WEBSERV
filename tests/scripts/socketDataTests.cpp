#include <iostream>
#include <list>

#include "SocketData.hpp"

#include "utils.cpp"

void	checkInvalidArgument(int minFd)
{
	int	data;
	printInfo("Try creating SocketData with invalid socket");

	data = 0;
	for (int i = minFd - 1; i > -3; i--)
	{
		try
		{
			SocketData	socketData(i, data, callback);
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
	int			data;

	data = 4;
	SocketData	socketData(4, data, callback);

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
	int						data;

	data = 0;
	printInfo("Try setting the wrong iterator");
	socketsData.push_front(SocketData(4, data, callback));
	socketsData.push_front(SocketData(5, data, callback));
	printInfo("Should output an error message :");
	socketsData.back().setIterator(socketsData.begin());
}

void	trySettingIteratorTwice()
{
	std::list<SocketData>	socketsData;
	int						data;

	data = 0;
	printInfo("try setting an iterator twice");
	socketsData.push_front(SocketData(4, data, callback));
	printInfo("Should output an error message :");
	socketsData.front().setIterator(socketsData.begin());
	socketsData.front().setIterator(socketsData.begin());
}

void	tryUsingIterator()
{
	std::list<SocketData>	socketsData;
	int						data;

	data = 4;
	printInfo("try using an iterator");
	socketsData.push_front(SocketData(4, data, callback));
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