#include <sstream>		// for ostringstream

#include "Status.hpp"	// for Status

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
	if (_code < 400)
		return (0);
	if (_errorPageSize != -1)
		return (_errorPageSize);
	size_t	size = 0;

	size += 154;
	size += _codeStrRepresentation.size();
	size += 96;
	size += _codeStrRepresentation.size();
	size += 14;
	size += _text.size();
	size += 21;
	return (size);
}

void	Status::buildErrorPage(void)
{
	if (_code < 400)
		return ;

	const size_t	size = getErrorPageSize();

	_errorPage.reserve(size);
	_errorPage +=
"<!DOCTYPE html> \
<html lang=\"fr\"> \
	<head> \
    	<meta charset=\"UTF-8\"> \
    	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"> \
    	<title>"
;
	_errorPage += _codeStrRepresentation;
	_errorPage += "</title> \
	</head> \
	<body style=\"font-family:sans-serif;text-align:center;padding:50px;\"> \
    	<h1>"
;
	_errorPage += _codeStrRepresentation;
	_errorPage += "</h1> \
    	<p>";
	_errorPage += _text;
	_errorPage += "</p> \
	</body> \
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
