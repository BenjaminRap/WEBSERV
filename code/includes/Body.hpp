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
public:
	Body(int fd, bool isBlocking);
	virtual ~Body();
	
	int			getFd() const;
	bool		getFinished() const;
	bool		getIsBlocking() const;
	
	virtual FlowState	writeBodyFromBufferToFile(FlowBuffer &flowBuffer) = 0;
	virtual FlowState	redirectBodyFromSocketToFile(FlowBuffer &flowBuffer, int socketFd) = 0;
};

#endif // !BODY_HPP
