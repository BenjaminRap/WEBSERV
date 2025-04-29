#ifndef HEADERS_HPP
# define HEADERS_HPP

# include <map>		// for std::map
# include <string>	// for std::string

# define LINE_END "\r\n"
# define LINE_END_LENGTH 2;

class Headers : public std::map<std::string, std::string>
{
private:
	Headers(const Headers& ref);

	Headers &operator=(const Headers& ref);
public:
	Headers(void);
	~Headers(void);

	const std::string*		getHeader(const std::string &key) const;

	int						parseHeader(const char *line, const char *end);
	size_t					getTotalSize(void) const;
};

std::ostream&	operator<<(std::ostream& o, const Headers& rhs);
std::string&	operator+=(std::string& dest, const Headers& src);

#endif // !HEADERS_HPP
