#ifndef EPOLL_HANDLER_HPP
# define EPOLL_HANDLER_HPP

# include <list>
# include <sys/epoll.h>
# include <vector>
# include <string>

# include "socketCommunication.hpp"

class	Configuration;
class	Host;
class	AEPollFd;
class	AFdData;

/**
 * @brief Manage the epoll functions : add or remove fd to the interest list and
 * execute the AEPollFd callback on events.
 */
class EPollHandler
{
private:
	/**
	 * @brief True if there is an instance of this class.
	 */
	static bool						_instanciated;

	/**
	 * @brief A list of the AEPollFd that are in the epoll interest list.
	 */
	std::list<AEPollFd*>			_ePollFds;
	/**
	 * @brief A list of the AEPollFd that needs to be removed from the
	 * interest list.
	 */
	std::list<const AEPollFd*>		_ePollFdsToRemove;
	/**
	 * @brief The epoll fd.
	 */
	int								_epfd;
	/**
	 * @brief The events array that will be passed at epoll_wait. It stores all
	 * the events that the epoll_wait function has detected.
	 */
	epoll_event						*_events;
	/**
	 * @brief The length of the _events array.
	 */
	unsigned int					_maxEvents;
	/**
	 * @brief When creating a socket unix connection, a socket unix file is
	 * created in the file system. This vector stores all the sockets path that
	 * have been created.
	 * It should be used to removed them in the destructor.
	 */
	std::vector<std::string>		_unixSocketsToRemove;

	EPollHandler(const EPollHandler& ref);
	EPollHandler(void);

	EPollHandler&	operator=(const EPollHandler &ref);

public:
	/**
	 * @brief Create a SocketHandler, allocate the event array and create the epoll fd.
	 * It also create a vector of sockets path to destroy, its size is the number of
	 * unix socket hosts in the Configuration class.
	 * This class can only has one instance.
	 * @param conf The EPollHandler use the configuration to initialize its variables.
	 * @throw Throw an error if the allocation failed (std::bad_alloc), epoll_create
	 * failed (std::exception) or this class already has an instance (std::logic_error).
	 */
	EPollHandler(const Configuration &conf);
	/**
	 * @brief Free the events array, close the sockets and close the epoll fd.
	 */
	~EPollHandler();

	/**
	 * @brief Add the FdData the the _ePollFds list.
	 * If the function fails, the FdData won't be destroyed.
	 *
	 * @return A boolean indicating if the fonction succeed.
	 */
	bool	addFdToList(AEPollFd &fdData);
	/**
	 * @brief Adds the fdData to the epoll interest list.
	 * with the events passed as arguments.
	 * If the function fails, the FdData won't be destroyed.
	 *
	 * @return A boolean indicating if the fonction succeed.
	 */
	bool	addFdToEpoll(AFdData& fdData, uint32_t events);
	/**
	 * @brief Call epoll_wait, call the callbacks of every AEPollFd
	 * notified by epoll and callremoveSocketsFromRemoveList.
	 *
	 * @return A boolean indicating if the fonction succeed.
	 */
	bool	handleIOEvents(void);
	/**
	 * @brief Bind the fd with the host variables. If the host family is AF_UNIX, 
	 * delete the socket at the host.sun_path, recreate a socket and add the socket
	 * path to the EPollHandler _unixSocketsToRemove vector.
	 * @param The fd to bind, should be a socket fd.
	 * @param host The host whose address will be used to bind the socket.
	 * @return 0 on success, -1 on error with an error message printed in the terminal.
	 */
	int		bindFdToHost(int fd, const Host &host);
	/**
	 * @brief For all the AEPollFd in the _ePollFdsToRemove, remove them
	 * from the _ePollFds list, removed them from epoll and delete them.
	 * Then clear the _ePollFdsToRemove list.
	 */
	void	removeSocketsFromRemoveList(void);
	/**
	 * @note it is usefull for  the ExecveException. Because in this case,
	 * the destructor shouldn't remove the unix sockets.
	 */
	void	clearUnixSocketsList(void);
	void	addFdToRemoveList(const AEPollFd& fdData);
};

#endif // !EPOLL_HANDLER_HPP
