#ifndef FD_DATA_HPP
# define FD_DATA_HPP

# include <stdint.h>
# include <list>
# include <stdexcept>

class SocketsHandler;

/**
 * @brief Represents all the data needed by a fd : the _callback function that
 * will be executed when the fd receive an event, its _iterator to remove itself
 * from the SocketsHandler list and the _data that will be passed to the _callback
 * functions.
 */
class FdData
{
protected:
	/**
	 * @brief The fd of the socket, it will be passed to the _callback function.
	 */
	int								_fd;
	/**
	 * @brief The data that will be passed to the _callback function.
	 */
	/**
	 * @brief The iterator of this FdData in the SocketsHandler list.
	 */
	std::list<FdData *>::iterator	_iterator;
	/**
	 * @brief True if the setIterator has been called with a good argument.
	 */
	bool							_isIteratorSet;
	/**
	 * @brief The class managing all the sockets.
	 */
	SocketsHandler					&_socketsHandler;

	FdData(int fd, SocketsHandler &socketsHandler);
private:
	FdData(void);
	FdData(const FdData &ref);

	FdData&	operator=(const FdData& ref);
public:
	virtual ~FdData(void);

	virtual void							callback(uint32_t events) = 0;

	int										getFd() const;
	const std::list<FdData *>::iterator		&getIterator() const;
	void									setIterator(const std::list<FdData *>::iterator &iterator);
};

#endif // !FD_DATA_HPP