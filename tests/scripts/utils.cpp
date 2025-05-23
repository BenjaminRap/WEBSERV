#include <stdint.h>
#include <iostream>
#include <cerrno>
#include <cstdio>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstring>

#include "AFdData.hpp"
#include "FlowBuffer.hpp"
#include "socketCommunication.hpp"

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

void	callback(AFdData &FdData, int *data, uint32_t events)
{
	(void)FdData;
	(void)data;
	(void)events;
}

bool	checkError(int errorFd)
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
	return (errorPrinted);
}

bool	redirectSTDERR(int (&tube)[2])
{
	if (checkError(pipe(tube), -1, "pipe() :"))
		return (false);
	if (checkError(dup2(tube[1], STDERR_FILENO), -1, "dup2() : "))
	{
		close(tube[0]);
		close(tube[1]);
		return (false);
	}
	if (addFlagsToFd(tube[0], O_NONBLOCK) == -1)
	{
		close(tube[0]);
		close(tube[1]);
		return (false);
	}
	return (true);
}

bool	checkContent(int srcFd, const char *expectedBuffer, size_t bufferCapacity)
{
	char	buffer[bufferCapacity];
	char	ignoreBuffer[1024];
	ssize_t	rd;

	rd = read(srcFd, buffer, bufferCapacity);
	if (rd == -1)
	{
		// std::cout << "error : " << strerror(errno) << std::endl;
		return (errno == EAGAIN && bufferCapacity == 0);
	}
	else if ((size_t)rd != bufferCapacity)
	{
		// std::cout << "not same length" << std::endl;
		return (false);
	}
	else
	{
		while (rd >= 0)
			rd = read(srcFd, ignoreBuffer, 1024);
		// write(STDIN_FILENO, buffer, bufferCapacity);
		// write(STDIN_FILENO, "\n", 1);
		// write(STDIN_FILENO, expectedBuffer, bufferCapacity);
		// write(STDIN_FILENO, "\n", 1);
		return (!std::memcmp(buffer, expectedBuffer, bufferCapacity));
	}
}

void	printSTDERR(int stderrReadFd)
{
	char	buffer[1024];
	ssize_t	rd = 1;

	do
	{
		rd = read(stderrReadFd, buffer, sizeof(buffer) - 1);
		if (rd != -1)
		{
			buffer[rd] = '\0';
			std::cout << buffer << std::endl;
		}
	}
	while (rd > 0);
}

void	ignoreSTDERR(int stderrReadFd)
{
	char	buffer[1024];
	ssize_t	rd = 1;

	do
	{
		rd = read(stderrReadFd, buffer, sizeof(buffer) - 1);
	}
	while (rd > 0);
}

std::string	&getFlowStateAsString(FlowState flowState)
{
	static std::string meaning[] = {"FLOW_ERROR", "FLOW_DONE", "FLOW_MORE", "FLOW_BUFFER_FULL"};

	return (meaning[flowState + 1]);
}

char 	*getFileInString(const char *path, size_t size, size_t &sizeResult)
{
	try
	{
		char	*str = new char[size];
		int		fd = open(path, O_RDONLY);

		if (fd == -1)
		{
			delete [] str;
			return (NULL);
		}
		const ssize_t rd = read(fd, str, size);
		close(fd);
		if (rd == -1)
		{
			delete [] str;
			return (NULL);
		}
		sizeResult = rd;
		return (str);
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (NULL);
	}
}

void	verifyFlowState(FlowState result, FlowState expectation)
{
	if (result == expectation)
		std::cout << getFlowStateAsString(result) << " ";
	else
		std::cout << getFlowStateAsString(result) << ", should be " << getFlowStateAsString(expectation);
	verify(result == expectation);
}

int	createFile(size_t size, char **fileContent)
{
	char	*buffer = new char[size];
	std::remove("/tmp/bufferFlowTests.txt");
	ssize_t fileFd = open("/tmp/bufferFlowTests.txt", O_CREAT | O_WRONLY, S_IRWXU);

	if (fileFd == -1)
	{
		delete [] buffer;
		return (-1);
	}
	ssize_t randomFd = open("/dev/random", O_RDONLY);
	if (randomFd == -1)
	{
		delete [] buffer;
		close(fileFd);
		return (-1);
	}
	if (read(randomFd, buffer, size) != (ssize_t)size
		|| write(fileFd, buffer, size) != (ssize_t)size)
	{
		close(fileFd);
		close(randomFd);
		delete [] buffer;
		return (-1);
	}
	close(randomFd);
	close(fileFd);
	*fileContent = buffer;
	return (open("/tmp/bufferFlowTests.txt", O_RDONLY));
}
