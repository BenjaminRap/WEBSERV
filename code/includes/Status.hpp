#ifndef STATUS_HPP
# define STATUS_HPP

# include <map>			// for std::map
# include <string>		// for std::string
# include <unistd.h>	// for ssize_t
# include <stdint.h>	// for uint16_t

enum StatusType
{
	STATUS_INFORMATIONAL	= 0b00001,
	STATUS_SUCESSFULL		= 0b00010,
	STATUS_REDIRECTION		= 0b00100,
	STATUS_CLIENT_ERROR		= 0b01000,
	STATUS_SERVER_ERROR		= 0b10000,
	STATUS_ERROR			= STATUS_CLIENT_ERROR | STATUS_SERVER_ERROR
};

class Status {
private:
	static const std::map<int, Status>	_statuses;

	const uint16_t		_code;
	const std::string	_codeStrRepresentation;
	const std::string	_text;
	std::string			_errorPage;
	ssize_t				_errorPageSize;

	Status(void);

	Status&					operator=(const Status &);

	size_t					getErrorPageSize(void) const;
	void					buildErrorPage(void);
public:
	Status(uint16_t code, const std::string &text);
	Status(const Status &ref);
	~Status(void);

	static bool				isCodeOfType(uint16_t code, StatusType type);
	static const Status&	getStatus(int code);

	bool					operator<(const Status& other);

	uint16_t				getCode(void) const;
	const std::string&		getCodeStringRepresentation(void) const;
	const std::string&		getText(void) const;
	const std::string&		getErrorPage(void) const;
	bool					isOfType(StatusType type) const;
};

bool	operator==(const Status &status, int code);

#endif // !STATUS_HPP
