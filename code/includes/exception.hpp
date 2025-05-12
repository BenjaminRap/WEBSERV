#ifndef	EXCEPTION_HPP
# define EXCEPTION_HPP

# include <string>
# include <sstream>

class ParsingException : public std::exception
{
public:
	ParsingException(const std::string& message) : _error(errorMsg(message))
	{
	}

	std::string errorMsg(const std::string& message) const
	{
		return ("Error parsing: " + message);
	}

	virtual const char* what() const throw()
	{
		return (_error.c_str());
	}
	virtual ~ParsingException() throw() {}

private:
	const std::string _error;
};

class ParsingLineException : public std::exception
{
public:
	ParsingLineException(const std::string& message, size_t line) :
		_error(errorMsg(message, line))
	{
	}

	std::string errorMsg(const std::string& message, size_t line) const
	{
		std::ostringstream oss;
		oss << "Error parsing:" << message << ", line: " << line;
		return (oss.str());
	}

	virtual const char* what() const throw()
	{
		return (_error.c_str());
	}
	virtual ~ParsingLineException() throw() {}

private:
	const std::string	_error;
};

class ParsingKeyWordAndLineException : public std::exception
{
public:
	ParsingKeyWordAndLineException(const std::string& message, size_t line, const std::string& word) :
		_error(errorMsg(message, word, line))
	{
	}

	std::string errorMsg(const std::string& message, const std::string& word, size_t line) const
	{
		std::ostringstream oss;
		oss << "Error parsing: " << message << " : " << word << ", line: " << line;
		return (oss.str());
	}

	virtual const char* what() const throw()
	{
		return (_error.c_str());
	}
	virtual ~ParsingKeyWordAndLineException() throw() {}

private:
	const std::string	_error;
};

#endif
