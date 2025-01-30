#ifndef CONNECTED_SOCKET_DATA_HPP
# define CONNECTED_SOCKET_DATA_HPP

# include "FdData.hpp"
# include "ResponsesHandler.hpp"
# include "RequestHandler.hpp"

class ConnectedSocketData : public FdData
{
private:
	ResponsesHandler	_responsesHandler;
	RequestHandler		_requestHandler;

	ConnectedSocketData(void);
	ConnectedSocketData(const FdData &ref);

	ConnectedSocketData&	operator=(const FdData& ref);
public:
	ConnectedSocketData(int fd, SocketsHandler &socketsHandler);
	~ConnectedSocketData(void);

	void	callback(uint32_t events);
};

#endif // !CONNECTED_SOCKET_DATA_HPP