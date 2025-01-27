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

	const FlowState	flowState = flowBuffer.redirectBufferContentToFd(fd, fdType);

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

	const FlowState flowState = flowBuffer.redirectFdContentToBuffer(fd, fdType);

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
	verifyFlowState(flowState, firstFlowResult);
	fdToBuffer(sockets[1], SOCKETFD, resultBuffer, bufferCapacity, buffer, numCharsWritten, secondFlowResult);
	delete [] resultBuffer;
}

void	fileToSocket(const char *path, size_t maxFileSize, size_t bufferSize, int (&sockets)[2], FlowState flowResult)
{
	FdType		fileType = FILEFD;
	FdType		socketType = SOCKETFD;
	size_t		fileSize;
	char		*file = getFileInString(path, maxFileSize, fileSize);
	const int	fileFd = open(path, O_RDONLY);
	char		*buffer = new char[bufferSize];

	if (fileFd == -1)
	{
		delete [] file;
		delete [] buffer;
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	FlowBuffer	flowBuffer(buffer, bufferSize, 0);
	const FlowState flowState = flowBuffer.redirectContent(fileFd, fileType, sockets[0], socketType);

	verify(checkContent(sockets[1], SOCKETFD, file, (fileSize < bufferSize) ? fileSize : bufferSize));
	verifyFlowState(flowState, flowResult);
	close(fileFd);
	delete [] file;
	delete [] buffer;
}

void	fileToBufer(size_t fileSize, size_t bufferSize, FlowState flowResult)
{
	FdType		fileType = FILEFD;
	char		*file = NULL;
	const int	fileFd = createFile(fileSize, &file);
	char		buffer[bufferSize];

	if (fileFd == -1)
	{
		delete [] file;
		std::cout << strerror(errno) << std::endl;
		return ;
	}
	FlowBuffer	flowBuffer(buffer, bufferSize, 0);
	const FlowState flowState = flowBuffer.redirectFdContentToBuffer(fileFd, fileType);

	verify(!std::memcmp(buffer, file, bufferSize < fileSize ? bufferSize : fileSize));
	verifyFlowState(flowState, flowResult);
	delete [] file;
	close(fileFd);
}

void	readAndWriteMultipleMessages(int (&sockets)[2], std::vector<std::string> &messages, size_t bufferSize)
{
	FdType		socketType = SOCKETFD;
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
		const FlowState flowState = flowBuffer.redirectFdContentToBuffer(sockets[1], socketType);
		flowBuffer.redirectBufferContentToFd(sockets[0], socketType);
		verify(checkContent(sockets[1], SOCKETFD, (*it).c_str(), (*it).size()));
		verifyFlowState(flowState, FLOW_MORE);
	}
}

void	redirectMultipleMessages(int (&sockets)[2], std::vector<std::string> &messages, size_t bufferSize)
{
	FdType		fileType = FILEFD;
	FdType		socketType = SOCKETFD;
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
		const FlowState flowState = flowBuffer.redirectContent(sockets[1], socketType, tube[1], fileType);
		checkContent(tube[0], FILEFD, (*it).c_str(), (*it).size());
		verifyFlowState(flowState, FLOW_MORE);
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
	size_t			hugeSize;
	char * const	hugeString = getFileInString("/dev/random", HUGE_STRING_LENGTH, hugeSize);

	if (hugeString == NULL)
		return (EXIT_FAILURE);

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1)
	{
		std::cout << strerror(errno) << '\n';
		return (EXIT_FAILURE);
	}
	printInfo("Print buffer in fd with small string");
	printInfo("FLOW_DONE because the flowBuffer buffer is smaller than the socket buffer");
	testBufferInFd((char*)"I am a test\n", 13, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_DONE);
	printInfo("Print buffer in fd with empty string");
	printInfo("FLOW_DONE because the flowBuffer buffer is smaller than the socket buffer");
	testBufferInFd((char*)"", 1, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_DONE);
	printInfo("Print buffer in fd with big string");
	printInfo("FLOW_DONE because the flowBuffer buffer is smaller than the socket buffer");
	testBufferInFd((char*)"Je suis un gros test azjhe iaziuehuazhexazex zhexiuahze \
	xiuhazeiuxhaz iuexhaziuehwiahzewhzoeixhazoeihaxez w ahiewuhaz azhexa  \
	waheh iuazhexiaz ea ewzoi a  azhew  aiuzhewi zewa iehwa iehaiewh az \
	ewiuahze iuwhazehwaz ewuhaez wahewua euwazeihwa iue iuw az ewiua ewhaew \
	haueh wuaheua ewiuahew aze wahezwahezwh aezhwah eiuw aezwhahezwa ewaz \
	ewahezwaezuw ahezwiahez wi haewha eziuwhoaez whaewhaz", 395, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_DONE);
	printInfo("Print buffer in fd with huge string");
	printInfo("FLOW_MORE_ because the flowBuffer buffer is larger than the socket buffer, so it can't be sent in one time");
	testBufferInFd(hugeString, hugeSize, sockets[0], SOCKETFD, sockets[1], SOCKETFD, FLOW_MORE);
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
	printInfo("buffer to socket to buffer with small string and equal buffer");
	printInfo("FLOW_DONE because the flowBuffer buffer is smaller than the socket buffer");
	printInfo("FLOW_BUFFER_FULL because the result buffer is of the same size that the message");
	bufferToSocketToBuffer((char*)"truc tralaala", 14, sockets, FLOW_DONE, FLOW_BUFFER_FULL);
	printInfo("buffer to socket to buffer with empty string and equal buffer");
	printInfo("FLOW_DONE because the flowBuffer buffer is smaller than the socket buffer");
	printInfo("FLOW_BUFFER_FULL because the result buffer is of the same size that the message");
	bufferToSocketToBuffer((char*)"", 1, sockets, FLOW_DONE, FLOW_BUFFER_FULL);
	printInfo("buffer to socket to buffer with big string and big buffer");
	printInfo("FLOW_DONE because the flowBuffer buffer is smaller than the socket buffer");
	printInfo("FLOW_BUFFER_FULL because the result buffer is of the same size that the message");
	bufferToSocketToBuffer((char*)"Je suis un gros test azjhe iaziuehuazhexazex zhexiuahze \
	xiuhazeiuxhaz iuexhaziuehwiahzewhzoeixhazoeihaxez w ahiewuhaz azhexa  \
	waheh iuazhexiaz ea ewzoi a  azhew  aiuzhewi zewa iehwa iehaiewh az \
	ewiuahze iuwhazehwaz ewuhaez wahewua euwazeihwa iue iuw az ewiua ewhaew \
	haueh wuaheua ewiuahew aze wahezwahezwh aezhwah eiuw aezwhahezwa ewaz \
	ewahezwaezuw ahezwiahez wi haewha eziuwhoaez whaewhaz", 395, sockets, FLOW_DONE, FLOW_BUFFER_FULL);
	printInfo("buffer to socket to buffer with huge string and huge buffer");
	printInfo("FLOW_MORE_ because the buffer is larger than the socket buffer");
	printInfo("FLOW_MORE_ because the buffer is larger than the socket buffer, so not of all the messgae has been sent, so the receive buffer is larger than what is received");
	bufferToSocketToBuffer(hugeString, hugeSize, sockets, FLOW_MORE, FLOW_MORE);
	printInfo("small file to socket with buffer smaller than file");
	fileToSocket("../tests/scripts/cors-test.html", 200, 100, sockets, FLOW_MORE);
	printInfo("small file to socket with buffer larger than file");
	fileToSocket("../tests/scripts/cors-test.html", 200, 1024, sockets, FLOW_MORE);
	printInfo("big file to socket with small buffer");
	fileToSocket("/var/log/dpkg.log.3.gz", 1024, 200, sockets, FLOW_MORE);
	printInfo("big file to socket with big buffer");
	fileToSocket("/var/log/dpkg.log.3.gz", 4096, 4096, sockets, FLOW_MORE);
	printInfo("empty file to socket with small buffer");
	fileToSocket("./../tests/scripts/flowBufferFiles/empty.txt", 0, 5, sockets, FLOW_DONE);
	printInfo("big file to smaller buffer");
	fileToBufer(1024, 500, FLOW_BUFFER_FULL);
	printInfo("small file to bigger buffer");
	fileToBufer(100, 500, FLOW_MORE);
	printInfo("empty file to buffer");
	fileToBufer(0, 5, FLOW_DONE);
	printInfo("Sending and receiving a message word by word");
	readAndWriteStringWordByWord(sockets);
	printInfo("Redirecting message word by word");
	redirectStringWordByWord(sockets);
	close(sockets[0]);
	close(sockets[1]);
	delete [] hugeString;
}