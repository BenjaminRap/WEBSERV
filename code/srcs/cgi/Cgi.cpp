#include <cstdlib>
#include <unistd.h>

#include "Cgi.hpp"
#include "exceptions.hpp"
#include "socketCommunication.hpp"

static void	closeTube(int (&tube)[2])
{
	checkError(close(tube[0]), -1, "close() : ");
	checkError(close(tube[1]), -1, "close() : ");
}

static void	cleanUp(int (&tubeSTDIN)[2], int (&tubeSTDOUT)[2])
{
	closeTube(tubeSTDIN);
	closeTube(tubeSTDOUT);
}

static void	createTubes(int (&tubeSTDIN)[2], int (&tubeSTDOUT)[2])
{
	if (pipe(tubeSTDIN) == -1)
		throw SystemError("pipe() : ");
	if (pipe(tubeSTDOUT) == -1)
	{
		closeTube(tubeSTDIN);
		throw SystemError("pipe() : ");
	}
}

static void	redirectProgram(int (&tubeSTDIN)[2], int (&tubeSTDOUT)[2])
{
	if (dup2(tubeSTDIN[0], STDIN_FILENO) == -1
		|| dup2(tubeSTDOUT[1], STDOUT_FILENO) == -1)
	{
		cleanUp(tubeSTDIN, tubeSTDOUT);
		std::exit(EXIT_FAILURE);
	}
}

Cgi::Cgi(const char *path, char * const * argv, char * const * envp)
{
	int			tubeSTDOUT[2];
	int			tubeSTDIN[2];
	
	createTubes(tubeSTDIN, tubeSTDOUT);
	_pid = fork();
	if (_pid == -1)
	{
		cleanUp(tubeSTDOUT, tubeSTDIN);
		throw SystemError("fork() : ");
	}
	if (_pid == 0)
	{
		redirectProgram(tubeSTDIN, tubeSTDOUT);
		cleanUp(tubeSTDIN, tubeSTDOUT);
		execve(path, argv, envp);
		std::exit(EXIT_FAILURE);
	}
	close(tubeSTDIN[0]);
	close(tubeSTDOUT[1]);
	_stdinFd = tubeSTDIN[1];
	_stdoutFd = tubeSTDOUT[0];
}

Cgi::~Cgi()
{
}	

pid_t	Cgi::getPid() const
{
	return (_pid);
}

int		Cgi::getStdinFd() const
{
	return (_stdinFd);
}

int		Cgi::getStdoutFd() const
{
	return (_stdoutFd);
}
