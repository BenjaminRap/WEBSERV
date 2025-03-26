#ifndef STATUS_HPP
# define STATUS_HPP

# include <string>		// for std::string
# include <unistd.h>	// for ssize_t
# include <stdint.h>	// for uint16_t

class Status {
private:
	const uint16_t		_code;
	const std::string	_codeStrRepresentation;
	const std::string	_text;
	std::string			_errorPage;
	ssize_t				_errorPageSize;

	Status(void);
	Status &operator=(const Status &);

	size_t	getErrorPageSize(void) const;

	void	buildErrorPage(void);
public:
	Status(uint16_t code, const std::string &text);
	Status(const Status &);
	~Status(void);

	int					getCode(void) const;
	const std::string&	getCodedStringRepresentation(void) const;
	const std::string&	getText(void) const;
	const std::string&	getErrorPage(void) const;
};

#endif // !STATUS_HPP
