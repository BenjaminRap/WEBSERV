#include <string>	// for std::string

char *duplicateString(const std::string &str);

void	setArgv(char* (&argv)[3], const std::string& interpreter, const std::string& cgiFile)
{
	if (interpreter.empty())
	{
		argv[0] = duplicateString(cgiFile);
		argv[1] = NULL;
	}
	else
	{
		argv[0] = duplicateString(interpreter);
		argv[1] = duplicateString(cgiFile);
		argv[2] = NULL;
	}
}
