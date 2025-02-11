#include <unistd.h>
#include <iostream>
#include <sys/wait.h>
#include <cstdlib>

pid_t	executeCGI(const char *path, char *const *argv, char *const *envp);

int	main(int argc, char * const *argv, char *const *envp)
{
	if (argc < 2)
	{
		std::cerr << "Wrong number of argument, minimum one for the script path" << std::endl;
		return (EXIT_FAILURE);
	}
	const pid_t pid = executeCGI(argv[1], argv + 1, envp);
	if (pid == -1)
	{
		std::cerr << "executeCGI() error" << std::endl;
		return (EXIT_FAILURE);
	}
	if (waitpid(pid, NULL, 0) != pid)
	{
		std::cerr << "waitpid() error" << std::endl;
		return (EXIT_FAILURE);
	}
	return (EXIT_SUCCESS);
}