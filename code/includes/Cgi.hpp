#ifndef CGI_HPP
# define CGI_HPP

# include <unistd.h>

class Cgi
{
private:
	pid_t	_pid;
	int		_stdinFd;
	int		_stdoutFd;
	
	Cgi();
	Cgi(const Cgi &ref);
	
	Cgi&	operator=(const Cgi &cgi);
public:
	Cgi(const char *path, char * const * argv, char * const * envp);
	~Cgi();

	pid_t	getPid() const;
	int		getStdinFd() const;
	int		getStdoutFd() const;
};

#endif // !CGI_HPP