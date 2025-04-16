#include "SocketsHandler.hpp"       // for SocketsHandler
#include "socketCommunication.hpp"  // for createAllServerSockets, getSignal...
class Configuration;

/**
 * @brief Create a server socket for hosts, they listen for new connection requests.
 * Then use epoll to check if the fds can be written / read.
 * @throw Can throw a std::bad_alloc, std::logic_error and std::exception, all coming
 * from the SocketsHandler constructor.
 * @param conf The configuration, it won't be changed.
 */
void	handleIOEvents(const Configuration &conf)
{
	SocketsHandler			socketsHandler(conf);

	createAllServerSockets(conf, socketsHandler);
	while (getSignalStatus() == NO_SIGNAL)
	{
		const int	nfds = socketsHandler.epollWaitForEvent(); 

		if (nfds == -1)
			break ;
		for (int i = 0; i < nfds; i++)
		{
			if (socketsHandler.closeIfConnectionStopped(i))
				continue;
			socketsHandler.callSocketCallback(i);
		}
	}
}
