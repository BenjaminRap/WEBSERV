#include <unistd.h>					// for fork(), pipe(), dup(), execve()
#include <csignal>                  // for signal, SIG_ERR, SIGINT, SIGPIPE
#include <cstdlib>                  // for EXIT_FAILURE
#include "socketCommunication.hpp"	// for checkError
#include <sys/wait.h>				// for waitpid()
#include "ARequestType.hpp"			// for Request
#include "requestStatusCode.hpp"	// for error code

void	close_on_err(int fd1, int fd2)
{
	close(fd1);
	close(fd2);
}

int	ARequestType::execCGI(const char *path, char **argv, char **env, int fd[2])
{
	//fork process
	pid_t	pid = fork();
	if (checkError(pid, -1, "fork() :"))
		return (close_on_err(fd[0], fd[1]), EXIT_FAILURE);

	if (pid == 0)
	{
		//set signals to default for the child process
		if (checkError(std::signal(SIGINT, SIG_DFL), SIG_ERR, "signal() : "))
			return (close_on_err(fd[0], fd[1]), EXIT_FAILURE);
		if (checkError(std::signal(SIGPIPE, SIG_DFL), SIG_ERR, "signal() : "))
			return (close_on_err(fd[0], fd[1]), EXIT_FAILURE);
		close(fd[1]);

			//redirecting cgi result to the pipe
		if (checkError(dup2(fd[1], STDOUT_FILENO), -1, "dup2() :"))
			return (close_on_err(fd[0], fd[1]), EXIT_FAILURE);
		close (fd[1]);
			//redirecting the other pipe for fun #pipex
		if (checkError(dup2(fd[0], STDIN_FILENO), -1, "dup2() :"))
			return (close_on_err(fd[0], STDOUT_FILENO), EXIT_FAILURE);
		close(fd[0]);

		//check acces and execute cgi
		if (checkError(access(path, X_OK), -1, "access() :"))
			return (close_on_err(STDIN_FILENO, STDOUT_FILENO), EXIT_FAILURE);
		if (checkError(execve(path, argv, env), -1, "execve() :"))
			return (close_on_err(STDIN_FILENO, STDOUT_FILENO), EXIT_FAILURE);
		std::exit(HTTP_INTERNAL_SERVER_ERROR);
	}
	else
	{
		return (pid);
	}
}

int	getCGIStatus(pid_t pid)
{
	int	status;
	if (waitpid(pid, &status, WNOHANG) == pid)
	{
		return (WEXITSTATUS(status));
	}
	else if (checkError(kill(pid, SIGKILL), -1, "kill() :"))
	{
		std::cerr << "kill() failed, be aware of zombies" << std::endl;
		return (-2);
	}
	else
		return (-1);
}
