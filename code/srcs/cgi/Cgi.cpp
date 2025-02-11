#include "Cgi.hpp"

Cgi::Cgi(pid_t pid, int stdinFd, int stdoutFd) :
	_pid(pid),
	_stdinFd(stdinFd),
	_stdoutFd(stdoutFd)
{
	
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
