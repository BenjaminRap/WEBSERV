#include <sys/socket.h>
#include <cstdlib>
#include <iostream>
#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <vector>

#include "FlowBuffer.hpp"

#include "utils.cpp"

FlowState	bufferToFd(int fd, FdType fdType, char *buffer, size_t bufferCapacity, size_t *numCharsWritten)
{
	FlowBuffer	flowBuffer(buffer, bufferCapacity, bufferCapacity);

	const FlowState	flowState = flowBuffer.redirectContentFromBuffer(fd, fdType);

	if (flowState == FLOW_ERROR)
		std::cout << strerror(errno) << '\n';
	if (flowBuffer.getNumCharsWritten() == 0)
		*numCharsWritten = bufferCapacity;
	else
		*numCharsWritten = flowBuffer.getNumCharsWritten();
	return (flowState);
}

void	fdToBuffer(int fd, FdType fdType, char *buffer, size_t bufferCapacity, char *expectedBuffer, size_t expectedLength, FlowState flowResult)
{
	FlowBuffer	flowBuffer(buffer, bufferCapacity, 0);

	const FlowState flowState = flowBuffer.redirectContentToBuffer(fd, fdType);

	if (flowState == FLOW_ERROR)
		std::cout << strerror(errno) << '\n';
	verify(flowBuffer.getBufferLength() == expectedLength && !std::memcmp(buffer, expectedBuffer, expectedLength));
	verifyFlowState(flowState, flowResult);
}

void	testBufferInFd(char *buffer, size_t bufferCapacity, int writeFd, FdType writeType, int readFd, FdType readType, FlowState flowResult)
{
	size_t	numCharsWritten;

	const FlowState flowState = bufferToFd(writeFd, writeType, buffer, bufferCapacity, &numCharsWritten);
	verify(checkContent(readFd, readType, buffer, numCharsWritten));
	verifyFlowState(flowState,flowResult);
}

void	bufferToSocketToBuffer(char *buffer, size_t bufferCapacity, int (&sockets)[2], FlowState firstFlowResult, FlowState secondFlowResult)
{
	size_t	numCharsWritten;

	char	*resultBuffer = new char[bufferCapacity];
	const FlowState flowState = bufferToFd(sockets[0], SOCKETFD, buffer, bufferCapacity, &numCharsWritten);
	verify(flowState == firstFlowResult);
	fdToBuffer(sockets[1], SOCKETFD, resultBuffer, bufferCapacity, buffer, numCharsWritten, secondFlowResult);
	delete [] resultBuffer;
}

void	fileToSocket(const char *path, size_t bufferSize, int (&sockets)[2], FlowState flowResult)
{
	size_t	fileSize;
	char		*file = getFileInString(path, bufferSize, fileSize);
	const int	fileFd = open(path, O_RDONLY);
	char		*buffer = new char[(bufferSize == 0 ? 1 : bufferSize)];

	if (fileFd == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	FlowBuffer	flowBuffer(buffer, (bufferSize == 0 ? 1 : bufferSize), 0);
	const FlowState flowState = flowBuffer.redirectContent(fileFd, FILEFD, sockets[0], SOCKETFD);

	verify(checkContent(sockets[1], SOCKETFD, file, fileSize));
	verifyFlowState(flowState, flowResult);
	close(fileFd);
	delete [] file;
	delete [] buffer;
}

void	fileToBufer(char *path, size_t bufferSize, FlowState flowResult)
{
	size_t	fileSize;
	const int	fileFd = open(path, O_RDONLY);
	char		*file = getFileInString(path, bufferSize, fileSize);
	char		buffer[(bufferSize == 0) ? 1 : bufferSize];

	if (fileFd == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	FlowBuffer	flowBuffer(buffer, (bufferSize == 0) ? 1 : bufferSize, 0);
	const FlowState flowState = flowBuffer.redirectContentToBuffer(fileFd, FILEFD);

	verify(fileSize == flowBuffer.getBufferLength() && !std::memcmp(buffer, file, fileSize));
	verifyFlowState(flowState, flowResult);
	delete [] file;
	close(fileFd);
}

void	readAndWriteMultipleMessages(int (&sockets)[2], std::vector<std::string> &messages, size_t bufferSize)
{
	char		buffer[bufferSize];
	FlowBuffer	flowBuffer(buffer, bufferSize, 0);

	for (std::vector<std::string>::const_iterator it = messages.begin(); it < messages.end(); it++)
	{
		const ssize_t written = send(sockets[0], (*it).c_str(), (*it).size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		if (written == -1)
		{
			std::cout << strerror(errno) << std::endl;
			return ;
		}
		const FlowState flowState = flowBuffer.redirectContentToBuffer(sockets[1], SOCKETFD);
		flowBuffer.redirectContentFromBuffer(sockets[0], SOCKETFD);
		verify(checkContent(sockets[1], SOCKETFD, (*it).c_str(), (*it).size()));
		verifyFlowState(flowState, FLOW_MORE_RECV);
	}
}

void	redirectMultipleMessages(int (&sockets)[2], std::vector<std::string> &messages, size_t bufferSize)
{
	char		buffer[bufferSize];
	FlowBuffer	flowBuffer(buffer, bufferSize, 0);
	int			tube[2];

	if (pipe(tube) == -1)
	{
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	if (fcntl(tube[0], F_SETFL, O_NONBLOCK) == -1)
	{
		std::cout << strerror(errno) << std::endl;
		close(tube[0]);
		close(tube[1]);
		return ;
	}
	for (std::vector<std::string>::const_iterator it = messages.begin(); it < messages.end(); it++)
	{
		const ssize_t written = send(sockets[0], (*it).c_str(), (*it).size(), MSG_DONTWAIT | MSG_NOSIGNAL);
		if (written == -1)
		{
			std::cout << strerror(errno) << std::endl;
			break ; 
		}
		const FlowState flowState = flowBuffer.redirectContent(sockets[1], SOCKETFD, tube[1], FILEFD);
		checkContent(tube[0], FILEFD, (*it).c_str(), (*it).size());
		verifyFlowState(flowState, FLOW_MORE_RECV);
	}
	close(tube[0]);
	close(tube[1]);
}

void	readAndWriteStringWordByWord(int (&sockets)[2])
{
	std::vector<std::string>	messages;

	messages.push_back("je ");
	messages.push_back("suis ");
	messages.push_back("un ");
	messages.push_back("test\n");
	readAndWriteMultipleMessages(sockets, messages, 7);
}

void	redirectStringWordByWord(int (&sockets)[2])
{
	std::vector<std::string>	messages;

	messages.push_back("je ");
	messages.push_back("suis ");
	messages.push_back("un ");
	messages.push_back("test\n");
	redirectMultipleMessages(sockets, messages, 7);
}

# define HUGE_STRING_LENGTH 1000000

int	main()
{
	int				sockets[2];
	char			buffer[1024];
	size_t			temp;
	char * const	hugeString = getFileInString("/dev/random", HUGE_STRING_LENGTH, temp);

	if (hugeString == NULL)
		return (EXIT_FAILURE);

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1)
	{
		std::cout << strerror(errno) << '\n';
		return (EXIT_FAILURE);
	}
	printInfo("Print buffer in fd with small string");
	testBufferInFd((char*)"I am a test\n", 13, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_DONE);
	printInfo("Print buffer in fd with empty string");
	testBufferInFd((char*)"", 1, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_DONE);
	printInfo("Print buffer in fd with big string");
	testBufferInFd((char*)"Je suis un gros test azjhe iaziuehuazhexazex zhexiuahze \
	xiuhazeiuxhaz iuexhaziuehwiahzewhzoeixhazoeihaxez w ahiewuhaz azhexa  \
	waheh iuazhexiaz ea ewzoi a  azhew  aiuzhewi zewa iehwa iehaiewh az \
	ewiuahze iuwhazehwaz ewuhaez wahewua euwazeihwa iue iuw az ewiua ewhaew \
	haueh wuaheua ewiuahew aze wahezwahezwh aezhwah eiuw aezwhahezwa ewaz \
	ewahezwaezuw ahezwiahez wi haewha eziuwhoaez whaewhaz", 395, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_DONE);
	printInfo("Print buffer in fd with huge string");
	testBufferInFd(hugeString, HUGE_STRING_LENGTH, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_MORE_SEND);
	printInfo("Try creating flow buffer with bufferlength superior to capacity");
	try
	{
		FlowBuffer(buffer, 100, 300);
		verify(false);
	}
	catch(const std::logic_error& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
	printInfo("Try creating a flow buffer with a buffer null");
	try
	{
		FlowBuffer(NULL, 100, 50);
		verify(false);
	}
	catch(const std::logic_error& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
	printInfo("Try creating a flow buffer with a capactity set to 0");
	try
	{
		FlowBuffer(buffer, 0, 0);
		verify(false);
	}
	catch(const std::logic_error& e)
	{
		std::cout << e.what() << '\n';
		verify(true);
	}
	printInfo("buffer to socket to buffer with small string and big buffer");
	bufferToSocketToBuffer((char*)"truc tralaala", 14, sockets, FLOW_DONE, FLOW_BUFFER_FULL);
	printInfo("buffer to socket to buffer with empty string and big buffer");
	bufferToSocketToBuffer((char*)"", 1, sockets, FLOW_DONE, FLOW_BUFFER_FULL);
	printInfo("buffer to socket to buffer with big string and big buffer");
	bufferToSocketToBuffer((char*)"Je suis un gros test azjhe iaziuehuazhexazex zhexiuahze \
	xiuhazeiuxhaz iuexhaziuehwiahzewhzoeixhazoeihaxez w ahiewuhaz azhexa  \
	waheh iuazhexiaz ea ewzoi a  azhew  aiuzhewi zewa iehwa iehaiewh az \
	ewiuahze iuwhazehwaz ewuhaez wahewua euwazeihwa iue iuw az ewiua ewhaew \
	haueh wuaheua ewiuahew aze wahezwahezwh aezhwah eiuw aezwhahezwa ewaz \
	ewahezwaezuw ahezwiahez wi haewha eziuwhoaez whaewhaz", 395, sockets, FLOW_DONE, FLOW_BUFFER_FULL);
	printInfo("buffer to socket to buffer with big string and big buffer");
	bufferToSocketToBuffer(hugeString, HUGE_STRING_LENGTH, sockets, FLOW_MORE_SEND, FLOW_MORE_RECV);
	printInfo("small file to socket with buffer smaller");
	fileToSocket("../tests/scripts/cors-test.html", 100, sockets, FLOW_DONE);
	printInfo("small file to socket with buffer larger");
	fileToSocket("../tests/scripts/cors-test.html", 200, sockets, FLOW_DONE);
	printInfo("big file to socket with small buffer");
	fileToSocket("/var/log/dpkg.log.3.gz", 5, sockets, FLOW_MORE_SEND);
	printInfo("big file to socket with big buffer");
	fileToSocket("/var/log/dpkg.log.3.gz", 1024, sockets, FLOW_DONE);
	printInfo("empty file to socket with small buffer");
	fileToSocket("./../tests/scripts/flowBufferFiles/empty.txt", 5, sockets, FLOW_DONE);
	printInfo("big file to smaller buffer");
	fileToBufer((char*)"/var/log/dpkg.log.3.gz", 1024, FLOW_BUFFER_FULL);
	printInfo("small file to bigger buffer");
	fileToBufer((char*)"./../tests/scripts/cors-test.html", 1024, FLOW_DONE);
	printInfo("empty file to buffer");
	fileToBufer((char*)"./../tests/scripts/flowBufferFiles/empty.txt", 5, FLOW_DONE);
	printInfo("Sending and receiving a message word by word");
	readAndWriteStringWordByWord(sockets);
	printInfo("Redirecting message word by word");
	redirectStringWordByWord(sockets);
	close(sockets[0]);
	close(sockets[1]);
	delete [] hugeString;
}