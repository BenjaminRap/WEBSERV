#include <unistd.h>
#include <iostream>
#include <csignal>

#include "socketCommunication.hpp"
#include "SocketsHandler.hpp"

/**
 * @brief Close all the fds and remove the fds of the interest list.
 */
void	cleanup(std::vector<SocketData> socketsData, int epfd, epoll_event *events)
{
	int	fd;

	for (std::vector<SocketData>::const_iterator ci = socketsData.begin(); ci < socketsData.end(); ci++)
	{
		fd = (*ci).getFd();
		checkError(epoll_ctl(epfd, EPOLL_CTL_DEL, fd, NULL), -1, "epoll_ctl() : ");
		checkError(close(fd), -1, "close() : ");
	}
	checkError(close(epfd), -1, "close() :");
	delete [] events;
}

bool	stop = false;

/**
 * @brief Create a server socket for all server configurations, they listen for
 * new connection request.
 * Then use poll to check if the fds can be written / ridden.
 * On a stdin input, close clean up and quit.
 * For now the error aren't taken into account.
 * @throw Can throw on a allocation failure
 * @param conf The configuration, it won't be changed
 */
void	handleIOEvents(const Configuration &conf)
{
	SocketsHandler			socketsHandler(conf.maxEvents);
	int						nfds;

	if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : ") == SIG_ERR)
		return ;
	createAllServerSockets(conf, socketsHandler);
	while (getSignalStatus() == NO_SIGNAL)
	{
		nfds = socketsHandler.epollWaitForEvent(); 
		if (nfds == -1)
			break ;
		for (int i = 0; i < nfds; i++)
		{
			socketsHandler.callSocketCallback(i);
		}
	}
}