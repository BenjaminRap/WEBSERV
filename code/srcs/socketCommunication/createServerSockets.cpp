#include <sys/socket.h>             // for listen, socket, AF_INET6, SOCK_ST...
#include <sys/un.h>                 // for sa_family_t
#include <exception>                // for exception
#include <iostream>                 // for char_traits, operator<<, basic_os...
#include <map>                      // for _Rb_tree_const_iterator, operator!=
#include <utility>                  // for pair
#include <vector>                   // for vector

#include "Configuration.hpp"        // for Configuration
#include "EPollHandler.hpp"         // for EPollHandler
#include "Host.hpp"                 // for Host
#include "ServerSocketData.hpp"     // for ServerSocketData
#include "socketCommunication.hpp"  // for closeFdAndPrintError, checkError

class ServerConfiguration;  // lines 19-19

/**
 * @brief Create a server socket, a socket used only to listen to connection creation request.
 * It create a socket, bind the host, port and family(IPV4 or IPV6), set the socket to
 * listen and return the fd.
 * If an error occur, the socket will be closed and the function return -1.
 * @param Host The configuration of the socket host (sockaddr).
 * @param maxConnection The max number of connection this socket will listen to.
 * @param reuseAddr A boolean to determine if the address can be reused just after
 * the socket being closed. Otherwise there is a delay (TIME_WAIT).
 */
static int	createServerSocket
(
	const Host &host,
	const Configuration	&conf,
	EPollHandler &ePollHandler
)
{
	const sa_family_t	family = host.getFamily();
	const int			fd = socket(family, SOCK_STREAM, 0);

	if (checkError(fd, -1, "socket() : "))
		return (-1);
	if (setReusableAddr(fd, conf.getReuseAddr()) == -1 // set the address reusable without delay
		|| (family == AF_INET6 && setIPV6Only(fd, true) == -1) // set the IPV6 sockets to only listen to IPV6
		|| ePollHandler.bindFdToHost(fd, host) == -1 // bind the socket to the address
		|| checkError(listen(fd, conf.getMaxConnectionBySocket()), -1, "listen() : ")) // set the socket to listening
	{
			closeFdAndPrintError(fd);
		return (-1);
	}
	return (fd);
}

void	createAllServerSockets
(
const Configuration &conf,
	EPollHandler &ePollHandler
)
{
	for (Configuration::const_iterator ci = conf.begin(); ci != conf.end(); ci++)
	{
		const Host								&host = ci->first;
		const std::vector<ServerConfiguration>	&serverConfigurations = ci->second;
		int										fd = createServerSocket(host, conf, ePollHandler);

		if (fd == -1)
			continue ;
		try
		{
			ServerSocketData& serverSocketData = *(new ServerSocketData(fd, ePollHandler, serverConfigurations, host));
			if (!ePollHandler.addFdToList(serverSocketData))
			{
				delete &serverSocketData;
				fd = -1;
			}
		}
		catch(const std::exception& e)
		{
			std::cerr << e.what() << '\n';
	  		closeFdAndPrintError(fd);
		}
	}
}
