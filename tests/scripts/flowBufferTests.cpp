#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "FlowBuffer.hpp"

std::string	&getFlowStateAsString(FlowState flowState)
{
	static std::string meaning[] = {"FLOW_ERROR", "FLOW_DONE", "FLOW_MORE_RECV", "FLOW_MORE_SEND", "FLOW_BUFFER_FULL"};

	return (meaning[flowState + 1]);
}

void	printBufferInFd(std::string &str, int fd, FdType fdType, char *buffer, size_t bufferSize)
{
	if (str.size() > bufferSize)
	{
		std::cout << "Test error, string too big" << std::endl;
		return ;
	}

	FlowBuffer	flowBuffer(buffer, bufferSize, str.size());

	std::memcpy(buffer, str.c_str(), str.size());

	const FlowState	flowState = flowBuffer.redirectContentFromBuffer(fd, fdType);

	std::cout << getFlowStateAsString(flowState) << std::endl;
	if (flowState == FLOW_ERROR)
		std::cout << strerror(errno) << '\n';
}

void	readFromFdToBuffer(int fd, FdType fdType, char *buffer, size_t bufferSize)
{
	FlowBuffer	flowBuffer(buffer, bufferSize, 0);

	const FlowState flowState = flowBuffer.redirectContentToBuffer(fd, fdType);

	std::cout << getFlowStateAsString(flowState) << std::endl;
	if (flowState == FLOW_ERROR)
		std::cout << strerror(errno) << '\n';
}

void	printBufferInSTDIN(std::string str)
{
	char		buffer[1024];

	printBufferInFd(str, STDIN_FILENO, FILEFD, buffer, 1024);
}

void	bufferToSocketToBuffer(std::string str, int (&sockets)[2])
{
	char	buffer[1024];

	printBufferInFd(str, sockets[0], SOCKETFD, buffer, 1024);
	readFromFdToBuffer(sockets[1], SOCKETFD, buffer, 1024);
}

void	fileToFd(std::string path, int fd, FdType fdType)
{
	const int	fileFd = open(path.c_str(), O_RDONLY);
	char		buffer[1024];

	if (fileFd == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	FlowBuffer	flowBuffer(buffer, 1024, 0);
	const FlowState flowState = flowBuffer.redirectContent(fileFd, FILEFD, fd, fdType);

	std::cout << getFlowStateAsString(flowState) << std::endl;
	if (flowState == FLOW_ERROR)
		std::cout << strerror(errno) << '\n';
	close(fileFd);
}

void	fileToBufer(std::string path)
{
	const int	fileFd = open(path.c_str(), O_RDONLY);
	char		buffer[1024];

	if (fileFd == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	FlowBuffer	flowBuffer(buffer, 1024, 0);
	const FlowState flowState = flowBuffer.redirectContentToBuffer(fileFd, FILEFD);

	std::cout << getFlowStateAsString(flowState) << std::endl;
	if (flowState == FLOW_ERROR)
		std::cout << strerror(errno) << '\n';
	close(fileFd);
}

int	main()
{
	int	sockets[2];

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1)
	{
		std::cout << strerror(errno) << '\n';
		return (EXIT_FAILURE);
	}
	printBufferInSTDIN("I am a test\n");
	printBufferInSTDIN("");
	bufferToSocketToBuffer("truc tralaala", sockets);
	bufferToSocketToBuffer("Truc bidule machin", sockets);
	fileToFd("../tests/scripts/cors-test.html", STDIN_FILENO, FILEFD);
	fileToFd("/var/log/dpkg.log.3.gz", STDIN_FILENO, FILEFD);
	fileToBufer("/var/log/dpkg.log.3.gz");
	close(sockets[0]);
	close(sockets[1]);
}