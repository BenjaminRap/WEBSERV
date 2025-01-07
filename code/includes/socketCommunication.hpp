#ifndef SOCKET_COMMUNICATION
# define SOCKET_COMMUNICATION

# define NO_SIGNAL -1
# include "Configuration.hpp"

class SocketData;

void	createAllServerSockets(const Configuration &conf, int epfd, std::vector<SocketData> &socketsData);
void	handleIOEvents(const Configuration &conf);
void	bzero(char *value, size_t size);
int		getSignalStatus();
void	signalHandler(int signal);

#include "errorCheck.tpp"

#endif // !SOCKET_COMMUNICATION