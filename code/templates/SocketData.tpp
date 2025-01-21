#include "SocketData.hpp"

/**
 * @brief Create a SocketData, it does not create the fd, add itself to the SocketsHandler
 * list nor add itself to the interest list.Those actions are done by the SocketsHandler.
 */
template <typename T>
SocketData::SocketData(int fd, T &data, void (&callback)(SocketData& socketData, T *data, uint32_t events)) :
	_fd(fd),
	_data(&data),
	_callback(reinterpret_cast<void (&)(SocketData& socketData, void *data, uint32_t events)>(callback)),
	_iterator(),
	_isIteratorSet(false),
	_canWrite(false),
	_responsesHandler()
{
	if (fd <= 3)
		throw std::invalid_argument("File descriptor is invalid in the SocketData constructor");
}