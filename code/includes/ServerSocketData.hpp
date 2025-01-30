#ifndef SERVER_SOCKET_DATA_HPP
# define SERVER_SOCKET_DATA_HPP

# include "FdData.hpp"

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