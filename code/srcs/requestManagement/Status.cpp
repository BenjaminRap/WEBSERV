#include <stddef.h>    // for size_t
#include <stdint.h>    // for uint16_t, uint8_t
#include <map>         // for map, operator==
#include <sstream>     // for basic_ostringstream, basic_ostream, char_traits
#include <stdexcept>   // for logic_error
#include <string>      // for basic_string, allocator, string
#include <utility>     // for pair
#include "Status.hpp"  // for Status, StatusType, operator==

/*******************************Constructors/Destructors*********************************/

std::string	getUInt16TInString(uint16_t value);

Status::Status(uint16_t code, const std::string &text) :
	_code(code),
	_codeStrRepresentation(getUInt16TInString(code)),
	_text(text),
	_errorPage(),
	_errorPageSize(-1)
{
	buildErrorPage();
}

Status::Status(const Status& ref) :
	_code(ref._code),
	_codeStrRepresentation(ref._codeStrRepresentation),
	_text(ref._text),
	_errorPage(ref._errorPage),
	_errorPageSize(ref._errorPageSize)
{
}

Status::~Status(void)
{
}

/*******************************Operator Overload**********************************************/


bool	Status::operator<(const Status& other)
{
	return (_code < other._code);
}


bool	operator==(const Status& status, int code)
{
	return (status.getCode() == code);
}

/********************************Private Methods**********************************************/

size_t	Status::getErrorPageSize(void) const
{
	if (isOfType(STATUS_ERROR) == false)
		return (0);
	if (_errorPageSize != -1)
		return (_errorPageSize);
	size_t	size = 0;

	size += 149;
	size += _codeStrRepresentation.size();
	size += 96;
	size += _codeStrRepresentation.size();
	size += 12;
	size += _text.size();
	size += 22;
	return (size);
}

void	Status::buildErrorPage(void)
{
	if (isOfType(STATUS_ERROR) == false)
		return ;

	const size_t	size = getErrorPageSize();

	_errorPage.reserve(size);
	_errorPage +=
"<!DOCTYPE html>\n\
<html lang=\"fr\">\n\
	<head>\n\
		<meta charset=\"UTF-8\">\n\
		<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n\
		<title>"
;
	_errorPage += _codeStrRepresentation;
	_errorPage += "</title>\n\
	</head>\n\
	<body style=\"font-family:sans-serif;text-align:center;padding:50px;\">\n\
		<h1>"
;
	_errorPage += _codeStrRepresentation;
	_errorPage += "</h1>\n\
		<p>";
	_errorPage += _text;
	_errorPage += "</p>\n\
	</body>\n\
</html>";
	_errorPageSize = _errorPage.size();
}

/*******************************Getters/Setters****************************************/

uint16_t	Status::getCode(void) const
{
	return (_code);
}

const std::string&	Status::getCodeStringRepresentation(void) const
{
	return (_codeStrRepresentation);
}

const std::string&	Status::getText(void) const
{
	return (_text);
}

const std::string&	Status::getErrorPage(void) const
{
	return (_errorPage);
}

bool	Status::isOfType(StatusType type) const
{
	return (Status::isCodeOfType(_code, type));
}

/**********************************Static method*************************************/


bool	Status::isCodeOfType(uint16_t code, StatusType type)
{
	const static StatusType	types[5] = {STATUS_INFORMATIONAL, STATUS_SUCESSFULL, STATUS_REDIRECTION,
		STATUS_CLIENT_ERROR, STATUS_SERVER_ERROR};
	const uint8_t			hundreds = code / 100 - 1;
	if (hundreds > 4)
		throw std::logic_error("Invalid code");
	return (types[hundreds] & type);
}


/**
 * @brief Returns the status text corresponding to the current code.
 * @throw If the current code doesn't correspond to a status text, throw a std::logic_error
 * @note This function should only be called if the request is done.
 */
const Status&	Status::getStatus(int code)
{
	const std::map<int, Status>::const_iterator	it = Status::_statuses.find(code);
	if (it == _statuses.end())
		throw std::logic_error("Unkown status code !");
	return (it->second);
}

/*********************************Helpers********************************************/

std::string	getUInt16TInString(uint16_t value)
{
	std::ostringstream	oss;

	oss << value;
	return (oss.str());
}
