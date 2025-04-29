#ifndef STATUS_HPP
# define STATUS_HPP

# include <map>			// for std::map
# include <string>		// for std::string
# include <unistd.h>	// for ssize_t
# include <stdint.h>	// for uint16_t

/**
 * @enum StatusType
 * @brief The differents type of status :
 * 		- STATUS_INFORMATIONAL
 * 		- STATUS_SUCESSFULL
 * 		- STATUS_REDIRECTION
 * 		- STATUS_CLIENT_ERROR
 * 		- STATUS_SERVER_ERROR
 * 		- STATUS_ERROR ( the two previous one combined)
 * @note Those values are bits and can be used with bitwise
 * operations.
 *
 */
enum StatusType
{
	STATUS_INFORMATIONAL	= 0b00001,
	STATUS_SUCESSFULL		= 0b00010,
	STATUS_REDIRECTION		= 0b00100,
	STATUS_CLIENT_ERROR		= 0b01000,
	STATUS_SERVER_ERROR		= 0b10000,
	STATUS_ERROR			= STATUS_CLIENT_ERROR | STATUS_SERVER_ERROR
};

/**
 * @class Status
 * @brief This class contains all the informations
 * relative to a status : its corresponding text, errorPage,
 * the representation of its code as a string or an int.
 *
 */
class Status {
private:
	/**
	 * @brief A map with code:Status with all the http standart
	 * statuses.
	 */
	static const std::map<int, Status>	_statuses;

	/**
	 * @brief The http code of the status between [100, 600[
	 */
	const uint16_t		_code;
	/**
	 * @brief The http text corresponding to the _code int.
	 */
	const std::string	_text;
	/**
	 * @brief The representation of the _code int value as
	 * a string.
	 */
	const std::string	_representation;
	/**
	 * @brief The errorPage builded from the _code and _text values.
	 */
	std::string			_errorPage;

	Status(void);

	Status&					operator=(const Status &);

	/**
	 * @brief Build the error page if the _code is an error.
	 * Otherwise, does nothing
	 */
	void					buildErrorPage(void);
	/**
	 * @brief Build the string representation of the status code + text.
	 */
	std::string				buildRepresentation(void);
public:
	Status(uint16_t code, const std::string &text);
	Status(const Status &ref);
	~Status(void);

	/**
	 * @brief A static method that check if a code corresponds to a type.
	 * A type can be a redirection, server error ...
	 * As StatusType are bits values, we can use bitwise operation on
	 * them. isCodeOfType(300, STATUS_REDIRECTION | STATUS_ERROR) will work.
	 *
	 * @return True if the code is of the type.
	 */
	static bool				isCodeOfType(uint16_t code, StatusType type);
	/**
	 * @brief A static method that returns the corresponding Status in the _statuses
	 * map
	 *
	 */
	static const Status&	getStatus(int code);

	bool					operator<(const Status& other);

	uint16_t				getCode(void) const;
	/**
	 * @brief The representation is the complete status line,
	 * containing the endLine.
	 */
	const std::string&		getRepresentation(void) const;
	size_t					getRepresentationSize(void) const;
	const std::string&		getText(void) const;
	const std::string&		getErrorPage(void) const;
	/**
	 * @brief Calls the static method isCodeOfType with this instance's code
	 * @ref isCodeOfType
	 *
	 */
	bool					isOfType(StatusType type) const;
};

bool	operator==(const Status &status, int code);

#endif // !STATUS_HPP
