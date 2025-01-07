#ifndef SOCKET_DATA
# define SOCKET_DATA

class SocketData
{
private:
	int		_fd;
	void	(&_callback)(int fd);
public:
	SocketData(int fd, void (&callback)(int fd));

	void	callback() const;
	int		getFd() const;
};

#endif // !SOCKET_DATA