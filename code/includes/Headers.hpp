#ifndef HEADERS_HPP
# define HEADERS_HPP

# include <map>		// for std::map
# include <string>	// for std::string

typedef std::map<std::string, std::string> HeaderMapType;

class Headers
{
private:
	HeaderMapType	_headers;

	Headers(const Headers& ref);

	Headers &operator=(const Headers& ref);
public:
	Headers(void);
	~Headers(void);

	const std::string*	getHeader(const std::string &key) const;
	const HeaderMapType	getMap(void) const;

	int					parseHeader(const char *line, const char *end);
	void				clear(void);
};

std::ostream& operator<<(std::ostream& o, const Headers& rhs);

#endif // !HEADERS_HPP
