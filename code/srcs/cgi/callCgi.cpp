#include <unistd.h>

pid_t	executeCGI(const char *path, char *const *argv, char *const *envp)
{
	const pid_t pid = fork();
	
	if (pid == 0)
	{
		if (execve(path, argv, envp) == -1)
			return (-1);
	}
	return (pid);	
}