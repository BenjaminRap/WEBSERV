#ifndef SOCKET_COMMUNICATION_HPP
# define SOCKET_COMMUNICATION_HPP

# define NO_SIGNAL -1

# include <stdint.h>
# include <cerrno>
# include <string>
# include <iostream>
# include <cstring>
# include <vector>
# include <sys/socket.h>

class SocketsHandler;
class Configuration;
typedef struct sockaddr sockaddr;

void	createAllServerSockets(const Configuration &conf, SocketsHandler &SocketsHandler);
void	handleIOEvents(const Configuration &conf);
int		getSignalStatus();
void	signalHandler(int signal);
int		getReturnCodeWithSignal();
int		setReusableAddr(int fd, bool reuseAddr);
int		setIPV6Only(int fd, bool isIPV6Only);
int		removeUnixSocketIfExists(const char sun_path[108]);
int		bindUnixSocket(int fd, const sockaddr *addr, socklen_t addrLen, \
			std::vector<std::string> &socketsToRemove);

template <typename ReturnType>
ReturnType	checkError(ReturnType value, ReturnType error, const std::string &errorPrefix);

# include "errorCheck.ipp"

#endif // !SOCKET_COMMUNICATION_HPP
