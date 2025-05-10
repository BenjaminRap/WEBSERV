#include <unistd.h>					// for fork(), pipe(), dup(), execve()
#include <csignal>                  // for signal, SIG_ERR, SIGINT, SIGPIPE
#include <cstdlib>                  // for EXIT_FAILURE
#include "socketCommunication.hpp"	// for checkError
#include <sys/wait.h>				// for waitpid()
#include "requestStatusCode.hpp"	// for error code

void	closeFds(int (&tube)[2])
{
	closeFdAndPrintError(tube[0]);
	closeFdAndPrintError(tube[1]);
}

void	closeFds(int fdA, int fdB)
{
	closeFdAndPrintError(fdA);
	closeFdAndPrintError(fdB);
}

int	replaceByProgram(const char *path, char *const * argv, char *const * env, int inFd, int outFd)
{
	if (checkError(std::signal(SIGINT, SIG_DFL), SIG_ERR, "signal() : ")
		|| checkError(std::signal(SIGPIPE, SIG_DFL), SIG_ERR, "signal() : "))
	{
		closeFds(inFd, outFd);
		return (-1);
	}

		//redirecting cgi result to the pipe
	if (checkError(dup2(outFd, STDOUT_FILENO), -1, "dup2() :"))
	{
		closeFds(inFd, outFd);
		return (-1);
	}
	closeFdAndPrintError(outFd);
		//redirecting the other pipe for fun #pipex
	if (checkError(dup2(inFd, STDIN_FILENO), -1, "dup2() :"))
	{
		closeFdAndPrintError(inFd);
		return (-1);
	}
	closeFdAndPrintError(inFd);

	//execute cgi
	checkError(execve(path, argv, env), -1, "execve() :");
	return (-1);
}

int	execCGI(const char *path, char * const * argv, char * const * env, int& inFd, int& outFd)
{
	int	tubeIn[2];
	int	tubeOut[2];

	if (pipe(tubeIn) == -1)
		return (-1);
	if (pipe(tubeOut) == -1)
	{
		closeFds(tubeIn);
		return (-1);
	}
	//fork process
	pid_t	pid = fork();
	if (checkError(pid, -1, "fork() :"))
	{
		closeFds(tubeIn);
		closeFds(tubeOut);
		return (-1);
	}

	if (pid == 0)
	{
		closeFds(tubeIn[0], tubeOut[1]);
		replaceByProgram(path, argv, env, tubeIn[1], tubeOut[0]);
		std::exit(EXIT_FAILURE);
	}
	else
	{
		closeFds(tubeIn[1], tubeOut[0]);
		inFd = tubeIn[0];
		outFd = tubeOut[1];
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
		return (-1);
	}
	else
		return (-1);
}
