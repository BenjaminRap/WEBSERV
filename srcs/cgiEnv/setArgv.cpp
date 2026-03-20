#include <cstring>
#include <string>	// for std::string

char *duplicateString(const std::string &str);

void	setArgv(const char* (&argv)[3], const std::string& interpreter, const std::string& cgiFile)
{
	std::memset(argv, 0, sizeof(argv));
	if (interpreter.empty())
		argv[0] = duplicateString(cgiFile);
	else
	{
		argv[0] = duplicateString(interpreter);
		argv[1] = duplicateString(cgiFile);
	}
}
