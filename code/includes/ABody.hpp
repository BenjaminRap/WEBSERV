#ifndef A_BODY_HPP
# define A_BODY_HPP

# include <stddef.h>

# include "FlowBuffer.hpp"

class ABody
{
private:
	int			_fd;
	bool		_finished;
	const  bool	_isBlocking;

	ABody(const ABody& ref);
	
	ABody&	operator=(const ABody& ref);
	
protected:
	void	setFinished();

	virtual ssize_t		writeToFd(const void *buffer, size_t bufferCapacity) = 0;
public:
	ABody(int fd, bool isBlocking);
	virtual ~ABody();
	
	int			getFd() const;
	bool		getFinished() const;
	bool		getIsBlocking() const;
	
	static ssize_t	callInstanceWriteToFd(ABody &body, const void *buffer, size_t bufferCapacity);
};

#endif // !A_BODY_HPP
