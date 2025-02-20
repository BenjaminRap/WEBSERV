#ifndef BODY_HPP
# define BODY_HPP

# include <stddef.h>

#include "FlowBuffer.hpp"

class Body
{
private:
	int		_fd;
	bool	_finished;

	Body(const Body& ref);
	
	Body&	operator=(const Body& ref);
	
protected:
	void	setFinished();
public:
	Body(int fd);
	virtual ~Body();
	
	int			getFd() const;
	bool		getFinished() const;
	
	virtual FlowState	writeBodyFromBufferToFile(FlowBuffer &flowBuffer) = 0;
	virtual FlowState	redirectBodyFromSocketToFile(FlowBuffer &flowBuffer, int socketFd) = 0;
};

#endif // !BODY_HPP