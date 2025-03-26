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

Status::~Status(void)
{
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

int					Status::getCode(void) const
{
	return (_code);
}

const std::string&	Status::getCodedStringRepresentation(void) const
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

/*********************************Helpers********************************************/

std::string	getUInt16TInString(uint16_t value)
{
	std::ostringstream	oss;

	oss << value;
	return (oss.str());
}
