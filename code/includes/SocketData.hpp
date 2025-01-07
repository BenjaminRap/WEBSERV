#ifndef SOCKET_DATA
# define SOCKET_DATA

class SocketData
{
private:
	int		_fd;
	void	*_data;
	void	(&_callback)(int fd, void *data);
public:
	SocketData(int fd, void *data, void (&callback)(int fd, void *data));

	void	callback() const;
	int		getFd() const;
};

#endif // !SOCKET_DATA