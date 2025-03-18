#ifndef CONNECTED_SOCKET_DATA_HPP
# define CONNECTED_SOCKET_DATA_HPP

#include <stdint.h>              // for uint32_t
	
#include "AFdData.hpp"            // for FdData
#include "RequestHandler.hpp"    // for RequestHandler
#include "ResponsesHandler.hpp"  // for ResponsesHandler

class SocketsHandler;

/**
 * @brief This class stores all the data needed by a connected Socket. This fd
 * process the request of a single client.
 */
class ConnectedSocketData : public AFdData
{
private:
	ResponsesHandler	_responsesHandler;
	RequestHandler		_requestHandler;

	ConnectedSocketData(void);
	ConnectedSocketData(const AFdData &ref);

	ConnectedSocketData&	operator=(const AFdData& ref);
	
	RequestState			processRequest(Response &response);
public:
	ConnectedSocketData(int fd, SocketsHandler &socketsHandler, const std::vector<ServerConfiguration> &serverConfiguration);
	~ConnectedSocketData(void);

	void	callback(uint32_t events);
};

#endif // !CONNECTED_SOCKET_DATA_HPP
