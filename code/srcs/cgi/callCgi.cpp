#include <unistd.h>
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

/* @brief Execute the program at path, with the arguments and environment variables
 * passed as parameters
 * @return A class containing the pid of the created child, the file descriptor
 * to read to and the file descriptor to write to.
*/ 
Cgi	&executeCGI(const char *path, char *const *argv, char *const *envp)
{
	const pid_t pid = fork();
	int			tubeSTDIN[2];
	int			tubeSTDOUT[2];
	
	if (pipe(tubeSTDIN) == -1)
		throw SystemError("pipe() : ");
	if (pipe(tubeSTDOUT) == -1)
	{
		closeTube(tubeSTDIN);
		throw SystemError("pipe() : ");
	}
	if (pid == 0)
	{
		if (dup2(STDIN_FILENO, tubeSTDIN[0]) == -1
			|| dup2(STDOUT_FILENO, tubeSTDOUT[1]) == -1)
		{
			cleanUp(tubeSTDIN, tubeSTDOUT);
			std::exit(EXIT_FAILURE);
		}
		cleanUp(tubeSTDIN, tubeSTDOUT);
		execve(path, argv, envp);
		std::exit(EXIT_FAILURE);
	}
	close(tubeSTDIN[0]);
	close(tubeSTDOUT[1]);
	return *(new Cgi(pid, tubeSTDIN[1], tubeSTDOUT[2]));
}