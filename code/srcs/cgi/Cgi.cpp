#include <cstdlib>

#include "Cgi.hpp"
#include "exceptions.hpp"

static void	closeTube(int (&tube)[2])
{
	close(tube[0]);
	close(tube[1]);
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
	if (dup2(STDIN_FILENO, tubeSTDIN[0]) == -1
		|| dup2(STDOUT_FILENO, tubeSTDOUT[1]) == -1)
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
