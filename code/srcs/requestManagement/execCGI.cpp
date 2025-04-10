#include <unistd.h>					// for fork(), pipe(), dup(), execve()
#include <csignal>                  // for signal, SIG_ERR, SIGINT, SIGPIPE
#include <cstdlib>                  // for EXIT_FAILURE
#include "socketCommunication.hpp"	// for checkError

int	execCGI(char **argv, char **env)
{
	pid_t	pid = fork();

	if (pid == 0)
	{
		if (checkError(std::signal(SIGINT, SIG_DFL), SIG_ERR, "signal() : "))
			return (EXIT_FAILURE);
		if (checkError(std::signal(SIGPIPE, SIG_DFL), SIG_ERR, "signal() : "))
			return (EXIT_FAILURE);
		
	}
}
