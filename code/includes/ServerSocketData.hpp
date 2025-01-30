#ifndef SERVER_SOCKET_DATA_HPP
# define SERVER_SOCKET_DATA_HPP

# include "FdData.hpp"

/**
 * @brief This class stores all the data needed by a server socket. A server socket
 * is a socket that listen on a specific host, and when it receives a request, 
 * create another fd connecteed to that client.
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