#ifndef SIZED_BODY_HPP
# define SIZED_BODY_HPP

# include "Body.hpp"

class SizedBody : public Body
{
private:
	size_t	_size;
	size_t	_numCharsWritten;
	
	SizedBody(const SizedBody& ref);
	SizedBody();
	
	SizedBody&	operator=(const SizedBody& ref);
public:
	SizedBody(int fd, size_t size, bool isBlocking);
	~SizedBody();

	ssize_t	writeToFd(const void *buffer, size_t bufferCapacity);
};

#endif // !SIZED_BODY_HPP
