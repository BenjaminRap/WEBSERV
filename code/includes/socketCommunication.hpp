#ifndef SOCKET_COMMUNICATION
# define SOCKET_COMMUNICATION

class Configuration;

void	createAllServerSockets(const Configuration &conf, int epfd, std::vector<int> &fds);
int		handleIOEvents(const Configuration &conf);
void	bzero(char *value, size_t size);

#endif // !SOCKET_COMMUNICATION