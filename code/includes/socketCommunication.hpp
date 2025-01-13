#ifndef SOCKET_COMMUNICATION_HPP
# define SOCKET_COMMUNICATION_HPP

# define NO_SIGNAL -1

class SocketsHandler;
class Configuration;

void	createAllServerSockets(const Configuration &conf, SocketsHandler &SocketsHandler);
void	handleIOEvents(const Configuration &conf);
int		getSignalStatus();
void	signalHandler(int signal);
int		getReturnCodeWithSignal();
int		setReusableAddr(int fd, bool reuseAddr);
int		setIPV6Only(int fd, bool isIPV6Only);
void	acceptConnection(int fd, void *data);

#include "errorCheck.tpp"

#endif // !SOCKET_COMMUNICATION_HPP