#include <netinet/in.h>             // for IPPROTO_IPV6, IPV6_V6ONLY
#include <sys/socket.h>             // for setsockopt, SOL_SOCKET, SO_REUSEADDR
#include <fcntl.h>					// for fcntl, F_GETFL, F_SETFL ...

#include "socketCommunication.hpp"  // for setIPV6Only, setReusableAddr

/**
 * @brief Set the socket SO_REUSEADDR option depending on reuseAddr.
 * @param fd The fd of the socket to set the SO_REUSEADDR option to.
 * @param reuseAddr True if the address can be reused shortly after, false otherwise.
 * Setting it to true can bring bugs but prevent the bind() : address already in use
 * error.
 * @return Return 0 on success and -1 on error, with an error message printed
 * in the terminal.
 */
int	setReusableAddr(int fd, bool reuseAddr)
{
	const int	reusable = reuseAddr ? 1 : 0;
	const int	returnValue = setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (void *)&reusable, sizeof(int));

	checkError(returnValue, -1, "setsockopt() : ");
	return (returnValue);
}

/**
 * @brief Sett the IPV6_V6ONLY socket option depending on isIPV6Only.
 * @param fd The fd of the socket to set the IPV6_V6Only option to.
 * @param isIPV6Only True if this socket can only listen to IPV6, false if it 
 * can listen to both IpV6 and IPV4.
 * @return Return 0 on success and -1 on error, with an error message printed in
 * the terminal.
 */
int	setIPV6Only(int fd, bool isIPV6Only)
{
	const int	ipv6Only = isIPV6Only ? 1 : 0;
	const int	returnValue = setsockopt(fd, IPPROTO_IPV6, IPV6_V6ONLY, (void *)&ipv6Only, sizeof(int));

	checkError(returnValue, -1, "setsockopt() : ");
	return (returnValue);
}

bool	addFlagsToFd(int fd, int statusFlags, int descriptorFlags)
{
	if (statusFlags != 0)
	{
		const int currentStatusFlags = fcntl(fd, F_GETFL);
		if (checkError(currentStatusFlags, -1, "fcntl() : "))
			return (false);
		int setStatusFlagsRet = fcntl(fd, F_SETFL, currentStatusFlags | statusFlags);
		if (checkError(setStatusFlagsRet, -1, "fcntl() : "))
			return (false);
	}
	if (descriptorFlags != 0)
	{
		const int currentDescriptorFlags = fcntl(fd, F_GETFD);
		if (checkError(currentDescriptorFlags, -1, "fcntl() : "))
			return (false);
		int setDescriptorFlagsRet = fcntl(fd, F_SETFD, currentDescriptorFlags | descriptorFlags);
		if (checkError(setDescriptorFlagsRet, -1, "fcntl() : "))
			return (false);
	}
	return (true);
}
