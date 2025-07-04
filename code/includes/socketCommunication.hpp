#ifndef SOCKET_COMMUNICATION_HPP
# define SOCKET_COMMUNICATION_HPP

# define NO_SIGNAL -1

# include <stdint.h>
# include <cerrno>
# include <string>
# include <cstring>
# include <vector>
# include <sys/socket.h>

class EPollHandler;
class Configuration;
typedef struct sockaddr sockaddr;

/**
 * @brief Create a server socket for each host of the conf variable.
 *
 * @param conf The configuration, it will not be changed.
 * @param EPollHandler The class that will be used to add sockets to the epoll
 * interest list.
 * @return A boolean indicating if the function succeed.
 */
bool	createAllServerSockets(const Configuration &conf, EPollHandler &epollHandler);
/**
 * @brief Create a server socket for hosts, they listen for new connection requests.
 * Then use epoll to check if the fds can be written / read.
 *
 * @param conf The configuration, it won't be changed.
 */
void	handleIOEvents(const Configuration &conf);
/**
 * @brief If a signal has been received, return it, otherwise, return NO_SIGNAL
 */
int		getSignalStatus();
/**
 * @brief Set the signalStatus value.
 */
void	signalHandler(int signal);
/**
 * @brief Return the program exit status, depending on the signal status.
 */
int		getReturnCodeWithSignal();

/**
 * @brief Add the signal handlers .
 *
 * @return A boolean indicating if the function succeeded.
 */
bool	addSignals(void);
/**
 * @brief Set the socket SO_REUSEADDR option depending on reuseAddr.
 * @param fd The fd of the socket to set the SO_REUSEADDR option to.
 * @param reuseAddr True if the address can be reused shortly after, false otherwise.
 * Setting it to true can bring bugs but prevent the bind() : address already in use
 * error.
 * @return Return 0 on success and -1 on error, with an error message printed
 * in the terminal.
 */
int		setReusableAddr(int fd, bool reuseAddr);
/**
 * @brief Sett the IPV6_V6ONLY socket option depending on isIPV6Only.
 * @param fd The fd of the socket to set the IPV6_V6Only option to.
 * @param isIPV6Only True if this socket can only listen to IPV6, false if it 
 * can listen to both IpV6 and IPV4.
 * @return Return 0 on success and -1 on error, with an error message printed in
 * the terminal.
 */
int		setIPV6Only(int fd, bool isIPV6Only);
/**
 * @brief Remove the sockets at sun_path.
 * @param sun_path The path of the socket in the file system.
 * @return If the sockets is removed or there is no sockets, returns 0. On
 * error, return -1 and print an error message in the terminal.
 */
int		removeUnixSocketIfExists(const char sun_path[108]);
/**
 * @brief Remove a preexisting socket, call bind and add the socket path to the
 * socketsToRemove vector.
 * @param fd The fd to bind, it should be a socket fd.
 * @param addr A pointer on the sockaddr structure who describe the address to
 * listen to.
 * @param addrLen The size of the addr structure (!= sizeof(sockaddr))
 * @param socketsToRemove The vector in which the socket path will be added
 * if the host family is AF_UNIX.
 * @throw It should not throw because the socketsToRemove vector should have a
 * preallocated size big enough. If the vector as been wrongly initialize it can
 * throw an std::bad_alloc.
 * @return 0 on success, -1 on eror with an error message printed in the terminal.
 */
int		bindUnixSocket(int fd, const sockaddr *addr, socklen_t addrLen, \
			std::vector<std::string> &socketsToRemove);
/**
 * @brief Add flags to a fd, keeping the previousflags.
 * The status flags are the flags beginning with O_, like O_NONBLOCK.
 * The descriptor flags are teh flags beginning with FD_, like FD_CLOEXEC.
 *
 * @return false on error, true otherwise.
 */
bool		addFlagsToFd(int fd, int statusFlags, int descriptorFlags);
/**
 * @brief Close the fd and print an error if it fails.
 * Ir prints close() : errno value
 *
 */
void	closeFdAndPrintError(int fd);

/**
 * @brief Check if the value is the same as the error value. If true, print the
 * error prefix and the strerror(errno). It can be used like that :
 * checkError(close(fd), -1, "close() : ");
 * This way, even if we can't manage a close error, we print an error message.
 * @return return true if there was an error, false otherwise.
 */
template <typename ReturnType>
bool	checkError(ReturnType value, ReturnType error, const char *errorPrefix);

# include "checkError.ipp"

#endif // !SOCKET_COMMUNICATION_HPP
