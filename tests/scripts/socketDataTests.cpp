#include <iostream>
#include <string>
#include <list>

#include "SocketData.hpp"

void	verify(bool test)
{
	if (test)
		std::cout << "\033[0;32mOK\033[0m\n";
	else
		std::cout << "\033[0;31mKO\033[0m\n";
}

void	callback(int fd, void *data)
{
	(void)fd;
	(void)data;
}

void	checkInvalidArgument(int minFd)
{
	std::cout << "Try creating SocketData with invalid socket" << std::endl;

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

	std::cout << "Try getting an unset iterator" << std::endl;
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

	std::cout << "Try setting the wrong iterator" << std::endl;
	socketsData.push_front(SocketData(4, (void *)NULL, callback));
	socketsData.push_front(SocketData(5, (void *)NULL, callback));
	std::cout << "Should output an error message :";
	socketsData.back().setIterator(socketsData.begin());
}

void	trySettingIteratorTwice()
{
	std::list<SocketData>	socketsData;

	std::cout << "try setting an iterator twice" << std::endl;
	socketsData.push_front(SocketData(4, (void *)NULL, callback));
	std::cout << "Should output an error message :";
	socketsData.front().setIterator(socketsData.begin());
	socketsData.front().setIterator(socketsData.begin());
}

void	tryUsingIterator()
{
	std::list<SocketData>	socketsData;

	std::cout << "try using an iterator" << std::endl;
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