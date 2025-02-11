#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <cstdlib>

#include "Cgi.hpp"

Cgi	&executeCGI(const char *path, char *const *argv, char *const *envp);

int	main(int argc, char * const *argv, char *const *envp)
{
	if (argc < 2)
	{
		std::cerr << "Wrong number of argument, minimum one for the script path" << std::endl;
		return (EXIT_FAILURE);
	}
	const Cgi	&cgi = executeCGI(argv[1], argv + 1, envp);
	const pid_t	pid = cgi.getPid();
	if (waitpid(pid, NULL, 0) != pid)
	{
		std::cerr << "waitpid() error" << std::endl;
	}
	close (cgi.getStdinFd());
	close (cgi.getStdoutFd());
	delete &cgi;
}