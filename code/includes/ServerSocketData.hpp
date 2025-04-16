#ifndef SERVER_SOCKET_DATA_HPP
# define SERVER_SOCKET_DATA_HPP

# include "AFdData.hpp"

/**
 * @brief This class stores all the data needed by a server socket. A server socket
 * is a socket that listens on a specific host, and when it receives a request, 
 * creates another fd connected to that client.
 */
class ServerSocketData : public AFdData
{
private:
	ServerSocketData(void);
	ServerSocketData(const AFdData &ref);

	ServerSocketData&	operator=(const AFdData& ref);
	
	/**
	 * @brief Accept a connection request, create a new fd, add it to the epoll interest
	 * list and add a ConnectedSocketData node to the socketsHandler list.
	 * @param events The events registered by epoll.
	 */
	void	acceptConnection(uint32_t events);
public:
	ServerSocketData(int fd , SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> &serverConfigurations);
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
