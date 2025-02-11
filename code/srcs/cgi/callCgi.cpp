#include <unistd.h>

bool	executeCGI(const char *path, char *const *argv, char *const *envp)
{
	const pid_t pid = fork();
	
	if (pid == -1)
		return (false);
	if (pid == 0)
	{
		if (execve(path, argv, envp) == -1)
			return (false);
	}
	else
	{

	}	
	return (true);
}