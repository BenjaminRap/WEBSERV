#ifndef SERVER_SOCKET_DATA_HPP
# define SERVER_SOCKET_DATA_HPP

# include "FdData.hpp"

/**
 * @brief This class stores all the data needed by a server socket. A server socket
 * is a socket that listens on a specific host, and when it receives a request, 
 * creates another fd connected to that client.
 */
class ServerSocketData : public FdData
{
private:
	ServerSocketData(void);
	ServerSocketData(const FdData &ref);

	ServerSocketData&	operator=(const FdData& ref);
	
	void	acceptConnection(uint32_t events);
public:
	ServerSocketData(int fd , SocketsHandler &socketsHandler);
	~ServerSocketData(void);

	void	callback(uint32_t events);
};

#endif // !SERVER_SOCKET_DATA_HPP