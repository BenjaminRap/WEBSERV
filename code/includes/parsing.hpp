#ifndef	PARSING_HPP
# define PARSING_HPP

# include <string>
# include <iostream>
# include <map>
# include <cstdlib>
# include <sstream>
# include <fstream>
# include <cstring>
# include <utility>
# include <netinet/in.h>
# include "Configuration.hpp"

# define WSPACE "\t\n\v\f\r "
# define SEP_WSPACE "\t\n\v\f\r ;"
# define SEP_WSPACE_ARG "\t\n\v\f\r ;{}"
# define DIGITS "0123456789"

# define ERROR_404_STR "/custom_404.html"
# define ERROR_404_INT 404
# define ERROR_405_STR "/custom_405.html"
# define ERROR_405_INT 405
# define ERROR_500_STR "/custom_500.html"
# define ERROR_500_INT 500

typedef struct ipv6_s {
    uint8_t ipv6[16];

	bool operator<(const ipv6_s& other) const {
        return std::memcmp(ipv6, other.ipv6, sizeof(uint8_t [16])) < 0;
	}

    bool operator==(const ipv6_s& other) const {
        return std::memcmp(ipv6, other.ipv6, sizeof(uint8_t [16])) == 0;
	}

	bool operator==(const uint8_t *other) const {
        return std::memcmp(this->ipv6, other, sizeof(uint8_t [16])) == 0;
	}

    uint8_t const (&getRef(void) const)[16] {
        return ipv6; 
    }

}	ipv6_t;

typedef struct ip_s
{
	std::vector<std::string>		unix_adrr;
	std::map<in_addr_t, in_port_t>	ipv4;
	std::map<ipv6_t, in_port_t>		ipv6;

	bool	operator<(const ip_s &other) const
	{
		return (this->unix_adrr < other.unix_adrr || this->ipv4 < other.ipv4 || this->ipv6 < other.ipv6);
	}
}	ip_t;

void	parse_file(Configuration &config, std::string &file);
void	parse_server(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::string &file, size_t &i, size_t &line);
void	skip_line(std::string &file, size_t &i, size_t &line);
void	skip_wspace(std::string &file, size_t &i, size_t &line);
short	real_atoi(std::string &file, size_t &i, size_t &line, short max, short len);
void	parse_ipv4(std::string &file, size_t &i, size_t &line, std::map<in_addr_t, in_port_t> &ip);
void	parse_host(std::string &file, size_t &i, size_t &line, ip_t &ip);
void	parse_ipv6(std::string &file, size_t &i, size_t &line, std::map<ipv6_t, in_port_t> &ip);
void	parse_ip_unix(std::string &file, size_t &i, size_t &line, std::vector<std::string> &ip);
void	parse_port(std::string &file, size_t &i, size_t &line, in_port_t &port);
void	parse_maxClientBodySize(std::string &file, size_t &i, size_t &line, size_t &maxClientBodySize);
void	parse_servername(std::string &file, size_t &i, size_t &line, std::vector<std::string> &serverNames);
void	parse_errorpages(std::string &file, size_t &i, size_t &line, std::map<unsigned short, std::string> &errorPages);
void	parse_route(std::string &file, size_t &i, size_t &line, std::map<std::string, Route> &routes);
void	parse_root(std::string &file, size_t &i, size_t &line, std::string &root);
void	parse_route_autoindex(std::string &file, size_t &i, size_t &line, bool &auto_index);
void	parse_route_index(std::string &file, size_t &i, size_t &line, std::vector<std::string> &index);
void	parse_route_accepted_method(std::string &file, size_t &i, size_t &line, std::vector<EMethods> &acceptedMethods);
void	parse_route_redirection(std::string &file, size_t &i, size_t &line, SRedirection &redirection);
void	parse_route_uploads(std::string &file, size_t &i, size_t &line, bool &acceptUploads);
void	ft_readfile(const char *path, std::string &buff);
void	insert_host(std::map<ip_t, std::vector<ServerConfiguration> > &conf, std::vector<std::string> \
&serverNames, std::map<unsigned short, std::string> &errorPages, size_t &maxClientBodySize, \
std::map<std::string, Route> &routes, std::string &root, ip_t &ip);

class   NumberOfArgumentException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Error: Wrong number of argument.");
		}
	};

	class   OpenFileException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Error: Couldn't open configuration file.");
		}
	};

	class   ReadFileException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Error: Couldn't read configuration file.");
		}
	};

	class   NoServerFoundException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Error: Couldn't find a server in configuration file.");
		}
	};

	class   MissingHostException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Error: Missing host");
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
				return ("Error: unexpected keyword:" + word + "\nline: " + oss.str());
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
				return ("Error: Multiple definition of: " + def + "\nline: " + oss.str());
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

	class UnclosedBraceException : public std::exception
	{
		public:
			UnclosedBraceException(size_t line) : line(line), error(errorMsg()) {}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: Unclosed brace\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~UnclosedBraceException() throw() {}

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
				return ("Error: Wrong server name: " + def + "\nline: " + oss.str());
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

	class PathNotFoundException : public std::exception
	{
		public:
			PathNotFoundException(size_t line) : line(line), error(errorMsg()) {}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: Path not found\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~PathNotFoundException() throw() {}

		private:
			size_t		line;
			std::string error;
	};

	class ParsedNumberOutOfRangeException : public std::exception
	{
		public:
			ParsedNumberOutOfRangeException(size_t line) : line(line), error(errorMsg()) {}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: Parsed number out of range\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~ParsedNumberOutOfRangeException() throw() {}

		private:
			size_t		line;
			std::string error;
	};

	class WrongPathException : public std::exception
	{
		public:
			WrongPathException(size_t line, std::string def) : line(line), def(def)
			{
				error = errorMsg();
			}

			std::string errorMsg() const
			{
				std::ostringstream oss;
				oss << line;
				return ("Error: Wrong path: " + def + "\nline: " + oss.str());
			}

			virtual const char* what() const throw()
			{
				return (error.c_str());
			}
			virtual ~WrongPathException() throw() {}

		private:
			size_t		line;
			std::string error;
			std::string def;
	};

	class   OutOfRangeException : public std::exception
	{
		public :
		
		virtual const char *what() const throw()
		{
			return ("Error: Element out of range.");
		}
	};

#endif