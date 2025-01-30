#include <errno.h>                  // for errno, ENOENT
#include <sys/socket.h>             // for bind, sockaddr, socklen_t
#include <sys/stat.h>               // for stat, S_IFMT, S_IFSOCK
#include <sys/un.h>                 // for sockaddr_un
#include <cstdio>                   // for remove
#include <cstring>                  // for strerror
#include <iostream>                 // for basic_ostream, char_traits, opera...
#include <string>                   // for basic_string, string
#include <vector>                   // for vector
	
#include "socketCommunication.hpp"  // for checkError, bindUnixSocket, remov...
/**
 * @brief Remove the sockets at sun_path.
 * @param sun_path The path of the socket in the file system.
 * @return If the sockets is removed or there is no sockets, returns 0. On
 * error, return -1 and print an error message in the terminal.
 */
int		removeUnixSocketIfExists(const char sun_path[108])
{
	struct stat fileInfos;

	if (stat(sun_path, &fileInfos) == -1)
	{
		if (errno == ENOENT)
			return (0);
		std::cerr << "stat() : " << strerror(errno) << std::endl;
		return (-1);
	}
	if ((fileInfos.st_mode & S_IFMT) != S_IFSOCK)
	{
		std::cerr << "Error : The file entered for the unix socket exists and isn't a unix socket file" << std::endl;
		return (-1);
	}
	if (std::remove(sun_path) != 0)
	{
		std::cerr << "remove () : " << strerror(errno) << std::endl;
		return (-1);
	}
	return (0);
}

/**
 * @brief Remove a preexisting socket, call bind and add the socket path to the
 * socketsToRemove vector.
 * @param fd The fd to bind, it should be a socket fd.
 * @param addr A pointer on the sockaddr structure who describe the address to
 * listen to.
 * @param addrLen The size of the addr structure (!= sizeof(sockaddr))
 * @param socketsToRemove The vector in which the socket path will be added
 * if the host family is AF_UNIX.
 * @throw It should not throw because the socketsToRemove vector should have a
 * preallocated size big enough. If the vector as been wrongly initialize it can
 * throw an std::bad_alloc.
 * @return 0 on success, -1 on eror with an error message printed in the terminal.
 */
int	bindUnixSocket
(
	int fd,
	const sockaddr *addr,
	socklen_t addrLen,
	std::vector<std::string> &socketsToRemove
)
{
	const sockaddr_un	*addrUnix;

	addrUnix = (const sockaddr_un *)addr;
	if (removeUnixSocketIfExists(addrUnix->sun_path) == -1)
		return (-1);
	if (checkError(bind(fd, addr, addrLen), -1, "bind() : "))
		return (-1);
	socketsToRemove.push_back(addrUnix->sun_path);
	return (0);
}