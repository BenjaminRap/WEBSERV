#ifndef EPOLL_HANDLER_HPP
# define EPOLL_HANDLER_HPP

# include <list>
# include <sys/epoll.h>
# include <vector>
# include <string>

# include "socketCommunication.hpp"

class	Configuration;
class	Host;
class	ASocketData;
class	AFdData;

/**
 * @brief Manage the epoll functions : add or remove fd to the interest list and
 * execute the socket callback on events.
 */
class EPollHandler
{
private:
	/**
	 * @brief True if there is an instance of this class.
	 */
	static bool					_instanciated;

	/**
	 * @brief A list of the FdData that are in the epoll interest list.
	 */
	std::list<ASocketData*>		_socketsData;
	std::list<ASocketData*>		_socketsToRemove;
	/**
	 * @brief The epoll fd.
	 */
	int							_epfd;
	/**
	 * @brief The events array that will be passed at epoll_wait. It stores all
	 * the events that the epoll_wait function has detected.
	 */
	epoll_event					*_events;
	/**
	 * @brief The length of the _events array.
	 */
	unsigned int				_maxEvents;
	/**
	 * @brief When creating a socket unix connection, a socket unix file is
	 * created in the file system. This vector stores all the sockets path that
	 * have been created.
	 * It should be used to removed them in the destructor.
	 */
	std::vector<std::string>	_unixSocketsToRemove;

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
	 * @brief Add the FdData the the _socketsdata list.
	 * If the function fails, the FdData won't be destroyed.
	 *
	 * @return -1 on error, otherwise 0.
	 */
	int		addFdToList(ASocketData &fdData);
	/**
	 * @brief Adds the fdData to the epoll interest list.
	 * If the function fails, the FdData won't be destroyed.
	 *
	 * @return -1 on error, 0 otherwise
	 */
	int		addFdToEpoll(AFdData& fdData, uint32_t events);
	/**
	 * @brief Call the callback of the socket, in the epoll events at eventIndex.
	 * @param eventIndex The index of the event to check, [0, eventCount] where eventCount 
	 * is the result of epoll_wait or epollWaitForEvent function.
	 */
	bool	callSocketsCallback(void);
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
	 * @brief Close a socket and remove if from the epoll interest list. It does not
	 * remove it from The _socketsData list.
	 * @param fd The fd of the socket to close.
	 */
	void	closeFdAndRemoveFromEpoll(int fd);
	/**
	 * @brief Removes an AFdData from the _socketsData list and delete it.
	 * @note The function doesn't have to removed it from the interest list
	 * as the AFdData destructor already call closeFdAndRemovedFromEpoll.
	 *
	 * @param pos 
	 */
	void	removeFdDataFromList(std::list<ASocketData*>::iterator pos);
	/**
	 * @note it is usefull for  the ExecveException. Because in this case,
	 * the destructor shouldn't remove the unix sockets.
	 */
	void	clearUnixSocketsList(void);
	void	addFdToRemoveList(ASocketData& fdData);
};

#endif // !EPOLL_HANDLER_HPP
