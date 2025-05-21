#include <sys/types.h>              // for pid_t
#include <sys/wait.h>               // for waitpid, WNOHANG
#include <unistd.h>                 // for dup2, pipe, execve, fork, STDIN_F...
#include <csignal>                  // for signal, SIG_DFL, SIG_ERR, kill
#include <cstdlib>                  // for WEXITSTATUS
#include <iostream>                 // for char_traits, basic_ostream, basic...

#include "exception.hpp"            // for ExecveException
#include "socketCommunication.hpp"  // for checkError, closeFdAndPrintError

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

void	replaceByProgram(const char *path, char *const * argv, char *const * env, int inFd, int outFd)
{
	// Reset the signals handler.
	if (checkError(std::signal(SIGINT, SIG_DFL), SIG_ERR, "signal() : ")
		|| checkError(std::signal(SIGTERM, SIG_DFL), SIG_ERR, "signal() : ")
		|| checkError(std::signal(SIGPIPE, SIG_DFL), SIG_ERR, "signal() : "))
	{
		closeFds(inFd, outFd);
		return ;
	}

	//redirecting cgi output to the pipe
	if (checkError(dup2(outFd, STDOUT_FILENO), -1, "dup2() :"))
	{
		closeFds(inFd, outFd);
		return ;
	}
	closeFdAndPrintError(outFd);

	//redirecting the cgi input to the pipe.
	if (checkError(dup2(inFd, STDIN_FILENO), -1, "dup2() :"))
	{
		closeFdAndPrintError(inFd);
		return ;
	}
	closeFdAndPrintError(inFd);

	//execute cgi
	execve(path, argv, env);
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
		closeFds(tubeIn[1], tubeOut[0]);
		replaceByProgram(path, argv, env, tubeIn[0], tubeOut[1]);
		throw ExecveException();
	}
	else
	{
		closeFds(tubeIn[0], tubeOut[1]);
		inFd = tubeIn[1];
		outFd = tubeOut[0];
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
