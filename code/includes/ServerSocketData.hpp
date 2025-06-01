#ifndef SERVER_SOCKET_DATA_HPP
# define SERVER_SOCKET_DATA_HPP

# include "AEPollFd.hpp"	// for ASocketData

# define SERVER_EVENTS (EPOLLIN | EPOLLERR)

class	Host;

/**
 * @brief This class stores all the data needed by a server socket. A server socket
 * is a socket that listens on a specific host, and when it receives a request, 
 * creates another fd connected to that client.
 */
class ServerSocketData : public AEPollFd
{
private:
	/**
	 * @brief On which host does this socket listen.
	 */
	const Host&								_host;
	const std::vector<ServerConfiguration>	&_serverConfigurations;

	ServerSocketData(void);
	ServerSocketData(const AEPollFd &ref);

	ServerSocketData&	operator=(const AEPollFd& ref);
	
	/**
	 * @brief Accept a connection request, create a new fd, add it to the epoll interest
	 * list and add a ConnectedSocketData node to the EpollHandler list.
	 * @param events The events registered by epoll.
	 */
	void	acceptConnection(uint32_t events);
public:
	ServerSocketData(int fd, EPollHandler &ePollHandler, const std::vector<ServerConfiguration> &serverConfigurations, const Host& host);
	~ServerSocketData(void);

	/**
	 * @brief The function called when epoll registers an event.
	 * This method only calls acceptConnection
	 *
	 * @param events 
	 */
	void	callback(uint32_t events);
};

#endif // !SERVER_SOCKET_DATA_HPP
