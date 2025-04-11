#include <unistd.h>					// for fork(), pipe(), dup(), execve()
#include <csignal>                  // for signal, SIG_ERR, SIGINT, SIGPIPE
#include <cstdlib>                  // for EXIT_FAILURE
#include "socketCommunication.hpp"	// for checkError
#include <sys/wait.h>				// for waitpid()

int	close_on_err(int fd1, int fd2)
{
	close(fd1);
	close(fd2);
	return (EXIT_FAILURE);
}

int	execCGI(const char *path, char **argv, char **env)
{

	//creating the pipe
	int		fd[2];
	if (checkError(pipe(fd), -1, "pipe() :"))
		return (EXIT_FAILURE);

	//fork process
	pid_t	pid = fork();
	if (checkError(pid, -1, "fork() :"))
		return (close_on_err(fd[0], fd[1]));

	if (pid == 0)
	{
		//set signals to default for the child process
		if (checkError(std::signal(SIGINT, SIG_DFL), SIG_ERR, "signal() : "))
			return (close_on_err(fd[0], fd[1]));
		if (checkError(std::signal(SIGPIPE, SIG_DFL), SIG_ERR, "signal() : "))
			return (close_on_err(fd[0], fd[1]));

		//redirecting cgi result to the pipe
		if (checkError(dup2(fd[1], STDOUT_FILENO), -1, "dup2() :"))
			return (close_on_err(fd[0], fd[1]));
		close(fd[1]);

		//check acces and execute cgi
		if (checkError(access(path, X_OK), -1, "access() :"))
			return (close_on_err(fd[0], STDOUT_FILENO));
		if (checkError(execve(path, argv, env), -1, "execve() :"))
			return (close_on_err(fd[0], STDOUT_FILENO));
	}
	else
	{
		//waiting for CGI to finish
		int	status;
		if (checkError(waitpid(pid, &status, 0), -1, "waitpid() :"))
			return (close_on_err(fd[0], fd[1]));
		if (status)
			return (close_on_err(fd[0], fd[1]));
	}
}
