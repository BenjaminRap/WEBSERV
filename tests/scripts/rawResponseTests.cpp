#include <sys/socket.h>
#include <cstdlib>

#include "RawResponse.hpp"

#include "utils.cpp"

void	testNoBody(char *firstPart, size_t firstPartLength, int (&sockets)[2], FlowState flowResult, bool isSentEqualToBuffer)
{
	RawResponse	response(firstPart, firstPartLength);
	const FlowState flowState = response.sendResponseToSocket(sockets[0]);
	verify(checkContent(sockets[1], SOCKETFD, firstPart, firstPartLength) == isSentEqualToBuffer);
	verifyFlowState(flowState, flowResult);
}

# define HUGE_STRING_LENGTH 1000000

int	main(void)
{
	size_t			hugeSize;
	int				sockets[2];
	char * const	hugeString = getFileInString("/dev/random", HUGE_STRING_LENGTH, hugeSize);

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) == -1)
		return (EXIT_FAILURE);
	printInfo("Small first part, no body");
	testNoBody((char*)"je suis un test", 16, sockets, FLOW_DONE, true);
	printInfo("big first part, no body");
	testNoBody((char*)"Je suis un gros test azjhe iaziuehuazhexazex zhexiuahze \
	xiuhazeiuxhaz iuexhaziuehwiahzewhzoeixhazoeihaxez w ahiewuhaz azhexa  \
	waheh iuazhexiaz ea ewzoi a  azhew  aiuzhewi zewa iehwa iehaiewh az \
	ewiuahze iuwhazehwaz ewuhaez wahewua euwazeihwa iue iuw az ewiua ewhaew \
	haueh wuaheua ewiuahew aze wahezwahezwh aezhwah eiuw aezwhahezwa ewaz \
	ewahezwaezuw ahezwiahez wi haewha eziuwhoaez whaewhaz", 395, sockets, FLOW_DONE, true);
	printInfo("FLOW_MORE_ because the flowBuffer buffer is larger than the socket buffer, so it can't be sent in one time");
	testNoBody(hugeString, hugeSize, sockets, FLOW_MORE, false);
	close(sockets[0]);
	close(sockets[1]);
	delete [] hugeString;
	exit (EXIT_SUCCESS);
}