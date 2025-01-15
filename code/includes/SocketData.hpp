#ifndef SOCKET_DATA_HPP
# define SOCKET_DATA_HPP

# include <list>
# include <stdexcept>

/**
 * @brief Represents all the data needed by a fd : the _callback function that
 * will be executed when the fd receive an event, its _iterator to remove itself
 * from the SocketsHandler list and the _data that will be passed to the _callback
 * functions.
 */
class SocketData
{
private:
	/**
	 * @brief The fd of the socket, it will be passed to the _callback function.
	 */
	int								_fd;
	/**
	 * @brief The data that will be passed to the _callback function.
	 */
	void							*_data;
	/**
	 * @brief The function that will be called when the epoll_wait detects an event.
	 */
	void							(&_callback)(int fd, void *data);
	/**
	 * @brief The iterator of this SocketData in the SocketsHandler list.
	 */
	std::list<SocketData>::iterator	_iterator;
	/**
	 * @brief True if the setIterator has been called with a good argument.
	 */
	bool							_isIteratorSet;

	SocketData(void);

	SocketData&	operator=(const SocketData& ref);
public:
	template <typename T> 
	SocketData(int fd, T *data, void (&callback)(int fd, T *data));
	SocketData(const SocketData &ref);
	~SocketData(void);

	void									callback() const;
	int										getFd() const;
	const std::list<SocketData>::iterator	&getIterator() const;
	void									setIterator(const std::list<SocketData>::iterator &iterator);
};

# include "SocketData.tpp"

#endif // !SOCKET_DATA_HPP