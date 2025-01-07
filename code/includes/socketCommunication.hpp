#ifndef SOCKET_COMMUNICATION
# define SOCKET_COMMUNICATION

# define NO_SIGNAL -1
# include "Configuration.hpp"

void	createAllServerSockets(const Configuration &conf, int epfd, std::vector<int> &fds);
int		handleIOEvents(const Configuration &conf);
void	bzero(char *value, size_t size);
int		getSignalStatus();
void	signalHandler(int signal);

#endif // !SOCKET_COMMUNICATION