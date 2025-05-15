#include "EPollHandler.hpp"         // for EPollHandler
#include "socketCommunication.hpp"  // for createAllServerSockets, getSignal...
class Configuration;

/**
 * @brief Create a server socket for hosts, they listen for new connection requests.
 * Then use epoll to check if the fds can be written / read.
 * @throw Can throw a std::bad_alloc, std::logic_error and std::exception, all coming
 * from the EPollHandler constructor.
 * @param conf The configuration, it won't be changed.
 */
void	handleIOEvents(const Configuration &conf)
{
	EPollHandler			ePollHandler(conf);

	createAllServerSockets(conf, ePollHandler);
	while (getSignalStatus() == NO_SIGNAL)
	{
		if (!ePollHandler.callSocketsCallback())
			break ;
	}
}
