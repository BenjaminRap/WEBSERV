#ifndef EXCEPTIONS_HPP
# define EXCEPTIONS_HPP

# include <exception>
# include <cerrno>
# include <string>
# include <string.h>

/**
 * @brief An exception that can be thrown when having an error that sets the errno.
 */
class SystemError : public std::exception {
private:
	const std::string _message;
public:
	/**
	 * @brief Creatte an exception with a message containingg : errorMessagePrefix
	 * + errno text
	 * @throw Can throw a std::bad_alloc.
	 */
	SystemError(const std::string &errorMessagePrefix) :
		_message(errorMessagePrefix + strerror(errno))
	{
	}
	
	~SystemError() throw()
	{
	}

	virtual const char *what () const throw()
	{
		return (_message.c_str());
	}
};

#endif // !EXCEPTIONS_HPP