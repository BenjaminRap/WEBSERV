#include <fcntl.h>
#include <cstring>
#include <cerrno>
#include <iostream>

#include "FlowBuffer.hpp"

#include "utils.cpp"

bool	redirectFileToFlowBuffer(FlowBuffer& flowBuffer, char *path)
{
	FdType		fileFd = FILEFD;
	FlowState	flowState;

	const int	fd = open(path, O_RDONLY);

	if (fd == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return (false);
	}
	do
	{
		flowState = flowBuffer.redirectFdContentToBuffer(fd, fileFd);
	} while (flowState == FLOW_MORE);
	close(fd);
	return (flowState != FLOW_ERROR);
}

void	printEverythingInFlowBuffer(FlowBuffer& flowBuffer)
{
	while (true)
	{
		char	*lineStart;
		ssize_t	length;

		if (!flowBuffer.getLine(&lineStart, &length))
			break ;
		std::cout << std::string(lineStart, (size_t)length) << std::endl;
	}
}

void	printFileLines(char *path, size_t bufferCapacity)
{
	char		buffer[bufferCapacity];
	FlowBuffer	flowBuffer(buffer, bufferCapacity, 0);

	redirectFileToFlowBuffer(flowBuffer, path);
	printEverythingInFlowBuffer(flowBuffer);
}

void	printStrLines(char *str, size_t strLength)
{
	FlowBuffer	flowBuffer(str, (strLength == 0) ? 1 : strLength, strLength);
	printEverythingInFlowBuffer(flowBuffer);
}

int	main(void)
{
	printInfo("file to buffer to getLine");
	printFileLines((char*)"../tests/scripts/cors-test.html", 1024);
	printInfo("buffer to getLine");
	printStrLines((char*)"je suis un test\n test truc\n test\n\ne\n", 37);
	printInfo("empty buffer to getLine");
	printStrLines((char*)"", 0);
}