#ifndef HEADERS_HPP
# define HEADERS_HPP

# include <list>
# include <map>		// for std::map
# include <string>	// for std::string

# define LINE_END "\r\n"
# define LINE_END_LENGTH 2

# define MAX_HEADER_COUNT 30

enum	InsertingType
{
	CONCAT_COMMA,
	CONCAT_SEMICOLON,
	UNIQUE
};

class Headers : private std::map<std::string, std::list<std::string> >
{
private:
	const static std::map<std::string, InsertingType>	_specialHeaders;

	Headers(const Headers& ref);

	Headers &operator=(const Headers& ref);
public:
	Headers(void);
	~Headers(void);

	const std::string*		getUniqueHeader(const std::string &key) const;

	int						parseHeader(const char *line, const char *end);
	size_t					getTotalSize(void) const;
	bool					addHeader(const std::string&key, const std::string& value);
};

std::ostream&	operator<<(std::ostream& o, const Headers& rhs);
std::string&	operator+=(std::string& dest, const Headers& src);

#endif // !HEADERS_HPP
