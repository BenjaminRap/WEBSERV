#include <cerrno>	// for errno
#include <unistd.h>	// for close
#include <string.h>	// for strerror
#include <iostream>	// for std::cerr, std::endl

void	closeFdAndPrintError(int fd)
{
	if (fd < 0)
		return ;
	const int ret = close(fd);
	if  (ret == -1)
	{
		std::cerr << "close() : " << strerror(errno) << std::endl;
		errno = 0;
	}
}
