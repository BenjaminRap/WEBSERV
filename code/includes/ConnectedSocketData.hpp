#ifndef CONNECTED_SOCKET_DATA_HPP
# define CONNECTED_SOCKET_DATA_HPP

# include <stdint.h>              // for uint32_t
	
# include "ASocketData.hpp"       // for ASocketData
# include "RequestHandler.hpp"    // for RequestHandler
# include "ResponsesHandler.hpp"  // for ResponsesHandler
# include "RequestContext.hpp"		  // for RequestVars

# define CONNECTED_EVENTS (EPOLLIN | EPOLLOUT | EPOLLERR | EPOLLRDHUP | EPOLLHUP)

class EPollHandler;

/**
 * @brief This class stores all the data needed by a connected Socket. This fd
 * process the request of a single client.
 */
class ConnectedSocketData : public ASocketData
{
private:
	/**
	 * @brief The class managing the queue of responses and the
	 * sending in the right order.
	 */
	ResponsesHandler	_responsesHandler;
	/**
	 * @brief The class managing the parsing and execution of
	 * the request.
	 */
	RequestHandler		_requestHandler;
	/**
	 * @brief True when this socket needs to stop receiving requests.
	 * When this is set to true, the ConnectedSocketData sends all the
	 * remaining responses and removes itself from the epoll interest
	 * list.
	 */
	bool				_closing;
	/**
	 * @brief A class containing all the variables necessary for the
	 * ARequestType.
	 */
	RequestContext		_requestContext;

	ConnectedSocketData(void);
	ConnectedSocketData(const ASocketData &ref);

	ConnectedSocketData&	operator=(const ASocketData& ref);
	
	/**
	 * @brief It redirect the data into the first part or the body, then call
	 * the requestHandler readRequest. It can reads multiples request so that
	 * if the client send multiples request in a single message, thhey can all
	 * be processed at the same time.
	 *
	 * @param response The result of the request will be written in it
	 * @return The state of the request. It can be any of the enum values.
	 */
	RequestState			processRequest(void);
public:
	ConnectedSocketData
	(
		int fd,
		EPollHandler &ePollHandler,
		const std::vector<ServerConfiguration> &serverConfiguration
	);
	~ConnectedSocketData(void);

	/**
	 * @brief If we receive an EPOLLIN, it reads the request.
	 * If we receive an EPOLLOUT, it write a response.
	 *
	 * @param events 
	 */
	void			callback(uint32_t events);
	RequestState	readNextRequests
	(
		Response& currentResponse,
		RequestState requestState
	);
};

#endif // !CONNECTED_SOCKET_DATA_HPP
