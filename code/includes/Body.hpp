#ifndef BODY_HPP
# define BODY_HPP

# include <stddef.h>

# include "FlowBuffer.hpp"

class Body
{
private:
	int			_fd;
	bool		_finished;
	const  bool	_isBlocking;

	Body(const Body& ref);
	
	Body&	operator=(const Body& ref);
	
protected:
	void	setFinished();

	virtual ssize_t		writeToFd(char *buffer, size_t bufferCapacity) = 0;
public:
	Body(int fd, bool isBlocking);
	virtual ~Body();
	
	int			getFd() const;
	bool		getFinished() const;
	bool		getIsBlocking() const;
	
	static ssize_t	callInstanceWriteToFd(int fd, char *buffer, size_t bufferCapacity, Body &body);
};

#endif // !BODY_HPP
