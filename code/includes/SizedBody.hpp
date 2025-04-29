#ifndef SIZED_BODY_HPP
# define SIZED_BODY_HPP

# include "ABody.hpp"

/**
 * @class SizedBody
 * @brief This class is a child of the ABody class.
 * It represents a body whose size is known.
 *
 */
class SizedBody : public ABody
{
private:
	/**
	 * @brief The size of the body.
	 */
	size_t	_size;
	/**
	 * @brief The number of characters already written.
	 */
	size_t	_numCharsWritten;
	
	SizedBody(const SizedBody& ref);
	SizedBody();
	
	SizedBody&	operator=(const SizedBody& ref);
public:
	SizedBody(int fd, size_t size);
	SizedBody(std::FILE* file, size_t size);
	SizedBody(size_t size);
	~SizedBody();

	/**
	 * @brief Write the content of buffer into the _fd.
	 * It writes until _size character has been written.
	 * The string stored in the buffer should be the size
	 * of the bufferCapacity.
	 *
	 * @return The numbber of characters writtenn with this call.
	 */
	ssize_t	writeToFd(const void *buffer, size_t bufferCapacity);
};

#endif // !SIZED_BODY_HPP
