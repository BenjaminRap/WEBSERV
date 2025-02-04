#ifndef BODY_HPP
# define BODY_HPP

# include <stddef.h>

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
	
	int		getFd() const;
	bool	getFinished() const;
};

#endif // !BODY_HPP