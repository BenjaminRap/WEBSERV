#ifndef	EXCEPTION_HPP
# define EXCEPTION_HPP

# include <string>
# include <sstream>

class CustomException : public std::exception
{
	public:
		CustomException(std::string message) : message(message)
		{
			error = errorMsg();
		}

		std::string errorMsg() const
		{
			return ("Error parsing: " + message);
		}

		virtual const char* what() const throw()
		{
			return (error.c_str());
		}
		virtual ~CustomException() throw() {}

	private:
		std::string error;
		std::string message;
	};

class CustomLineException : public std::exception
{
	public:
		CustomLineException(std::string message, size_t line) : line(line)
		{
			this->message = message;
			error = errorMsg();
		}

		std::string errorMsg() const
		{
			std::ostringstream oss;
			oss << line;
			return ("Error parsing: " + message + "\nline: " + oss.str());
		}

		virtual const char* what() const throw()
		{
			return (error.c_str());
		}
		virtual ~CustomLineException() throw() {}

	private:
		size_t		line;
		std::string error;
		std::string message;
};

class CustomKeyWordAndLineException : public std::exception
{
	public:
		CustomKeyWordAndLineException(std::string message, size_t line, std::string word) \
		: line(line), word(word)
		{
			this->message = message;
			error = errorMsg();
		}

		std::string errorMsg() const
		{
			std::ostringstream oss;
			oss << line;
			return ("Error parsing: " + message + ": " + word + "\nline: " + oss.str());
		}

		virtual const char* what() const throw()
		{
			return (error.c_str());
		}
		virtual ~CustomKeyWordAndLineException() throw() {}

	private:
		size_t		line;
		std::string word;
		std::string error;
		std::string message;
};

#endif