#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <vector>
# include <map>
# include <algorithm>
# include <iostream>
# include <fstream>
# include <string>
# include <sstream> 

# include "ServerConfiguration.hpp"

# define DEFAULT_CONFIG_PATH "../documentation/configuration.txt"
# define WSPACE "\t\n\v\f\r "

# define SEP_WSPACE "\t\n\v\f\r ;"

# define DIGITS "0123456789"

void	ft_readfile(const char *path, std::string &buff);

/// @brief This class describe all the configuration parsed from the configuration
/// file. This program should process multiples 'servers', each having their own
/// configuration
class Configuration
{

	public :

	Configuration(const char *path);
	~Configuration(void);

	class   NumberOfArgumentException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Wrong number of argument.");
		}
	};

	class   OpenFileException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Couldn't open configuration file.");
		}
	};

	class   ReadFileException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Couldn't read configuration file.");
		}
	};

	class   NoServerFoundException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Couldn't find a server in configuration file.");
		}
	};

	class UnexpectedKeyWordException : public std::exception
	{
		public:
			UnexpectedKeyWordException(size_t line, std::string word)
				: line(line), word(word), error(errorMsg()) {}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: unexpected keyword: " + word + "\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~UnexpectedKeyWordException() throw() {}

		private:
			size_t		line;
			std::string word;
			std::string error;
	};

	class WrongIpFormatException : public std::exception
	{
		public:
			WrongIpFormatException(size_t line)
				: line(line), error(errorMsg()) {}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: IP Host/Port Format \nline: " + oss.str() + "\nExpected format: listen 0.0.0.0:0000");
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~WrongIpFormatException() throw() {}

		private:
			size_t		line;
			std::string error;
	};

	class MultipleDefinitionException : public std::exception
	{
		public:
			MultipleDefinitionException(size_t line, std::string def) : line(line), def(def)
			{
				error = errorMsg();
			}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Multiple definition of: " + def + "\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~MultipleDefinitionException() throw() {}

		private:
			size_t		line;
			std::string error;
			std::string def;
	};

	class MissingSemiColonException : public std::exception
	{
		public:
			MissingSemiColonException(size_t line) : line(line), error(errorMsg()) {}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: Missing semi-colon\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~MissingSemiColonException() throw() {}

		private:
			size_t		line;
			std::string error;
	};

	class MissingErrorPageException : public std::exception
	{
		public:
			MissingErrorPageException(size_t line) : line(line), error(errorMsg()) {}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: Missing error page\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~MissingErrorPageException() throw() {}

		private:
			size_t		line;
			std::string error;
	};

	class WrongServerNameException : public std::exception
	{
		public:
			WrongServerNameException(size_t line, std::string def) : line(line), def(def)
			{
				error = errorMsg();
			}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Wrong server name: " + def + "\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~WrongServerNameException() throw() {}

		private:
			size_t		line;
			std::string error;
			std::string def;
	};

	private :

	std::vector<ServerConfiguration>	ServerConfigurations;

	void	parse_file(std::string &file);
	void	parse_server(std::string &file, size_t &i, size_t &line);
	void	skip_line(std::string &file, size_t &i, size_t &line);
	void	skip_wspace(std::string &file, size_t &i, size_t &line);
	short	real_atoi(std::string &file, size_t &i, size_t &line, short max, short len);
	void	parse_ip(std::string &file, size_t &i, size_t &line, uint32_t &host);
	void	parse_port(std::string &file, size_t &i, size_t &line, uint16_t &port);
	void	parse_maxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize);
	void	parse_servername(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames);
	void	parse_errorpages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages);

	Configuration(void);
};

#endif // !CONFIGURATION_HPP
