#ifndef SOCKET_DATA_HPP
# define SOCKET_DATA_HPP

# include <stdint.h>
# include <list>
# include <stdexcept>
# include <queue>

# include "Response.hpp"

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
	void							(&_callback)(SocketData &socketData, void *data, uint32_t events);
	/**
	 * @brief The iterator of this SocketData in the SocketsHandler list.
	 */
	std::list<SocketData>::iterator	_iterator;
	/**
	 * @brief True if the setIterator has been called with a good argument.
	 */
	bool							_isIteratorSet;
	/**
	 * @brief True if we can write in the socket. It is set true when we get a
	 * EPOLLOUT, but false when there is a EAGAIN.
	 */
	bool							_canWrite;
	/**
	 * @brief A queue of all the responses. They will be written to the client in
	 * the same order that the client send the requests.
	 */
	std::queue<RawResponse>			_responses;

	SocketData(void);

	SocketData&	operator=(const SocketData& ref);
public:
	template <typename T> 
	SocketData(int fd, T &data, void (&callback)(SocketData &socketData, T *data, uint32_t events));
	SocketData(const SocketData &ref);
	~SocketData(void);

	void									callback(uint32_t events);
	int										getFd() const;
	const std::list<SocketData>::iterator	&getIterator() const;
	void									setIterator(const std::list<SocketData>::iterator &iterator);
	void									setCanWrite(bool canWrite);
};

# include "SocketData.tpp"

#endif // !SOCKET_DATA_HPP