#ifndef SOCKET_COMMUNICATION_HPP
# define SOCKET_COMMUNICATION_HPP

# define NO_SIGNAL -1

# include <stdint.h>
# include <cerrno>
# include <string>
# include <iostream>
# include <cstring>

class SocketsHandler;
class Configuration;
class SocketData;

void	createAllServerSockets(const Configuration &conf, SocketsHandler &SocketsHandler);
void	handleIOEvents(const Configuration &conf);
int		getSignalStatus();
void	signalHandler(int signal);
int		getReturnCodeWithSignal();
int		setReusableAddr(int fd, bool reuseAddr);
int		setIPV6Only(int fd, bool isIPV6Only);
void	acceptConnection(SocketData& socketData, SocketsHandler *socketsHandler, uint32_t events);

template <typename ReturnType>
ReturnType	checkError(ReturnType value, ReturnType error, const std::string &errorPrefix);

# include "errorCheck.tpp"

#endif // !SOCKET_COMMUNICATION_HPP