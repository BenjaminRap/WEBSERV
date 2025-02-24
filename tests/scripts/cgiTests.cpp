#include <cerrno>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <cstdlib>

#include "Cgi.hpp"

void	readAndCheckValue(int fd, const std::string &value)
{
	char	buffer[1024];
	const ssize_t	rd = read(fd, buffer, sizeof(buffer) - 1);
	if (rd == -1)
	{
		std::cout << "Error reading : " << strerror(errno) << std::endl;
		return ;
	}
	buffer[rd] = '\0';
	std::cout << ((value == std::string(buffer)) ? "OK" : "KO") << '\n';
}

void	writeAndReadValue(const Cgi &cgi, const std::string &value)
{
	write(cgi.getStdinFd(), value.c_str(), 2);
	readAndCheckValue(cgi.getStdoutFd(), value);
}

int	main(int argc, char * const *argv, char *const *envp)
{
	if (argc < 2)
	{
		std::cerr << "Wrong number of argument, minimum one for the script path" << std::endl;
		return (EXIT_FAILURE);
	}
	try
	{
		const Cgi	cgi(argv[1], argv + 1, envp);
		const pid_t	pid = cgi.getPid();
		writeAndReadValue(cgi, "4");
		if (waitpid(pid, NULL, 0) != pid)
			std::cerr << "waitpid() error" << std::endl;
		close(cgi.getStdinFd());
		close(cgi.getStdoutFd());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}
