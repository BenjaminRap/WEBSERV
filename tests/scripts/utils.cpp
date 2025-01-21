#include <iostream>

#include "SocketData.hpp"

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

void	callback(SocketData &socketData, int *data, uint32_t events)
{
	(void)socketData;
	(void)data;
	(void)events;
}