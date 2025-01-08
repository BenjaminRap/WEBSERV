#include <iostream>

#include "SocketsHandler.hpp"

void	verify(bool test)
{
	if (test)
		std::cout << "\033[0;32mOK\033[0m\n";
	else
		std::cout << "\033[0;31mKO\033[0m\n";
}

void	tryCreatingMultipleInstance()
{
	SocketsHandler	socketsHandler(100);

	std::cout << "Try creating multiples instance" << std::endl;
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
	std::cout << "try creating and destroying multiple instances" << std::endl;
	{
		SocketsHandler	socketsHandler(100);
	}
	{
		SocketsHandler	socketsHandler(100);
	}
	verify(true);
}

int	main(void)
{
	tryCreatingMultipleInstance();
	createAndDestroyMultipleInstance();
}