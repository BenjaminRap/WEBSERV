#ifndef SIZED_BODY_HPP
# define SIZED_BODY_HPP

# include "Body.hpp"

class SizedBody : public Body
{
private:
	size_t	_size;
	size_t	_numCharsWritten;
	
	SizedBody(const SizedBody& ref);
	
	SizedBody&	operator=(const SizedBody& ref);
public:
	SizedBody(int fd, size_t size);
	~SizedBody();
	
	size_t	getSize() const;
	void	addCharsWritten(size_t add);
	
	static ssize_t	writeToFile(int fd, char *buffer, size_t bufferCapacity, SizedBody &sizedBody);
};

#endif // !SIZED_BODY_HPP