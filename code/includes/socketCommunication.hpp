#ifndef SOCKET_COMMUNICATION_HPP
# define SOCKET_COMMUNICATION_HPP

# define NO_SIGNAL -1
# include "Configuration.hpp"

class SocketsHandler;

void	createAllServerSockets(const Configuration &conf, SocketsHandler &SocketsHandler);
void	handleIOEvents(const Configuration &conf);
void	bzero(char *value, size_t size);
int		getSignalStatus();
void	signalHandler(int signal);
int		getReturnCodeWithSignal();
int		setReusableAddr(int fd, bool reuseAddr);
int		setIPV6Only(int fd, bool isIPV6Only);
void	acceptConnection(int fd, void *data);

#include "errorCheck.tpp"

#endif // !SOCKET_COMMUNICATION_HPP