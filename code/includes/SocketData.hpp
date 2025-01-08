#ifndef SOCKET_DATA
# define SOCKET_DATA

class SocketData
{
private:
	int								_fd;
	void							*_data;
	void							(&_callback)(int fd, void *data);
	std::list<SocketData>::iterator	_iterator;
public:
	SocketData(int fd, void *data, void (&callback)(int fd, void *data));

	void									callback() const;
	int										getFd() const;
	const std::list<SocketData>::iterator	&getIterator() const;
	void									setIterator(std::list<SocketData>::iterator iterator);
};

#endif // !SOCKET_DATA