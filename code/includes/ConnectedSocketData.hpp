#ifndef CONNECTED_SOCKET_DATA_HPP
# define CONNECTED_SOCKET_DATA_HPP

#include <stdint.h>              // for uint32_t
	
#include "FdData.hpp"            // for FdData
#include "RequestHandler.hpp"    // for RequestHandler
#include "ResponsesHandler.hpp"  // for ResponsesHandler

class SocketsHandler;

/**
 * @brief This class stores all the data needed by a connected Socket. This fd
 * process the request of a single client.
 */
class ConnectedSocketData : public FdData
{
private:
	ResponsesHandler	_responsesHandler;
	RequestHandler		_requestHandler;

	ConnectedSocketData(void);
	ConnectedSocketData(const FdData &ref);

	ConnectedSocketData&	operator=(const FdData& ref);
	
	RequestState			processRequest();
public:
	ConnectedSocketData(int fd, SocketsHandler &socketsHandler);
	~ConnectedSocketData(void);

	void	callback(uint32_t events);
};

#endif // !CONNECTED_SOCKET_DATA_HPP