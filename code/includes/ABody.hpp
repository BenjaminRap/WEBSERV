#ifndef A_BODY_HPP
# define A_BODY_HPP

# include <sys/types.h>	// for ssize_t

class ABody
{
private:
	int			_fd;
	bool		_finished;

	ABody(const ABody& ref);
	
	ABody&	operator=(const ABody& ref);
	
protected:
	void	setFinished();

	virtual ssize_t		writeToFd(const void *buffer, size_t bufferCapacity) = 0;
public:
	ABody(int fd);
	virtual ~ABody();
	
	int			getFd() const;
	bool		getFinished() const;
	
	static ssize_t	callInstanceWriteToFd(ABody &body, const void *buffer, size_t bufferCapacity);
};

#endif // !A_BODY_HPP
