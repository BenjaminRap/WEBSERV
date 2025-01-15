#include "SocketData.hpp"

/**
 * @brief Create a SocketData, it does not create the fd, add itself to the SocketsHandler
 * list nor add itself to the interest list.Those actions are done by the SocketsHandler.
 */
template <typename T>
SocketData::SocketData(int fd, T *data, void (&callback)(int fd, T *data)) :
	_fd(fd),
	_data(data),
	_callback(reinterpret_cast<void (&)(int fd, void *data)>(callback)),
	_iterator(),
	_isIteratorSet(false)
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");
}