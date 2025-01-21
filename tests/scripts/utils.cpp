#include <iostream>
#include <cerrno>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

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

void	checkError(int errorFd)
{
	char	buffer[1024];
	ssize_t	rd;
	bool	errorPrinted;

	errorPrinted = false;
	while (true)
	{
		rd = read(errorFd, buffer, sizeof(buffer));
		if (rd > 0)
			errorPrinted = true;
		else
			break ;
		write(STDOUT_FILENO, buffer, rd);
	}
	std::cout << '\n';
	verify(errorPrinted);
}

bool	redirectSTDERR(int (&tube)[2])
{
	if (pipe(tube) == 1)
	{
		std::cerr << "Failed to pipe in tests\n";
		return (false);
	}
	if (dup2(tube[1], STDERR_FILENO) == -1)
	{
		std::cerr << "Failed to dup2 in tests\n";
		close(tube[0]);
		close(tube[1]);
		return (false);
	}
	if (fcntl(tube[0], F_SETFL, fcntl(tube[0], F_GETFL) | O_NONBLOCK) == -1)
	{
		std::cerr << "Failed to fcntl in tests\n";
		close(tube[0]);
		close(tube[1]);
		return (false);
	}
	return (true);
}