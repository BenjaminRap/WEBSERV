#include <unistd.h>
#include <cstdlib>

static void	closeTube(int tube[2])
{
	close(tube[0]);
	close(tube[1]);
}

pid_t	executeCGI(const char *path, char *const *argv, char *const *envp)
{
	const pid_t pid = fork();
	int			tubeSTDIN[2];
	int			tubeSTDOUT[2];
	
	if (pipe(tubeSTDIN) == -1)
		return (-1);
	if (pipe(tubeSTDOUT) == -1)
	{
		closeTube(tubeSTDIN);
		return (-1);
	}
	if (pid == 0)
	{
		if (dup2(STDIN_FILENO, tubeSTDIN[0]) == -1
			|| dup2(STDOUT_FILENO, tubeSTDOUT[1]) == -1)
		{
			closeTube(tubeSTDIN);
			closeTube(tubeSTDOUT);
			std::exit(EXIT_FAILURE);
		}
		closeTube(tubeSTDIN);
		closeTube(tubeSTDOUT);
		execve(path, argv, envp);
		std::exit(EXIT_FAILURE);
	}
	closeTube(tubeSTDIN);
	closeTube(tubeSTDOUT);
	return (pid);	
}