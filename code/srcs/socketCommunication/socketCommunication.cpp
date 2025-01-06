#include <sys/epoll.h>
#include <unistd.h>
#include <iostream>

#include "Configuration.hpp"
#include "socketCommunication.hpp"

/**
 * @brief Add the STDIN to the epoll interest fds, but not to the fds vector, because
 * it should not be closed.
 * Set the event to EPOLLIN.
 */
int	addSTDIN(int epfd)
{
	epoll_event	event;

	event.events = EPOLLIN;
	event.data.fd = STDIN_FILENO;
	return (epoll_ctl(epfd, EPOLL_CTL_ADD, STDIN_FILENO, &event));
}

/**
 * @brief Close all the fds and remove the fds of the interest list.
 */
void	cleanup(std::vector<int> fds, int epfd, epoll_event *events)
{
	for (std::vector<int>::const_iterator ci = fds.begin(); ci < fds.end(); ci++)
	{
		epoll_ctl(epfd, EPOLL_CTL_DEL, *ci, NULL);
		close(*ci);
	}
	close(epfd);
	delete [] events;
}

/**
 * @brief Create a server socket for all server configurations, they listen for
 * new connection request.
 * Then use poll to check if the fds can be written / ridden.
 * On a stdin input, close clean up and quit.
 * For now the error aren't taken into account.
 * @throw Can throw on a allocation failure
 * @param conf The configuration, it won't be changed
 */
int	handleIOEvents(const Configuration &conf)
{
	int					epfd;
	std::vector<int>	fds;
	epoll_event			*events;
	int					nfds;
	bool				stop;

	events = new epoll_event[conf.maxEvents]();
	epfd = epoll_create(1);
	if (epfd == -1)
	{
		delete [] events;
		return (-1);
	}
	createAllServerSockets(conf, epfd, fds);
	stop = false;
	if (addSTDIN(epfd) == -1)
	{
		cleanup(fds, epfd, events);
		return (-1);
	}
	while (!stop)
	{
		nfds = epoll_wait(epfd, events, conf.maxEvents, -1);
		if (nfds == -1)
			break ;
		for (int i = 0; i < nfds; i++)
		{
			if (events[i].data.fd == STDIN_FILENO)
			{
				stop = true;
				break ;
			}
		}
	}
	cleanup(fds, epfd, events);
	return (0);
}