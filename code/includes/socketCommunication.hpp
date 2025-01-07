#ifndef SOCKET_COMMUNICATION
# define SOCKET_COMMUNICATION

# define NO_SIGNAL -1
# include "Configuration.hpp"

class SocketsHandler;

void	createAllServerSockets(const Configuration &conf, SocketsHandler &SocketsHandler);
void	handleIOEvents(const Configuration &conf);
void	bzero(char *value, size_t size);
int		getSignalStatus();
void	signalHandler(int signal);
int		getReturnCodeWithSignal();

#include "errorCheck.tpp"

#endif // !SOCKET_COMMUNICATION