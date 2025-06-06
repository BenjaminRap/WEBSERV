#include <stddef.h>    	// for size_t
#include <stdint.h>    	// for uint16_t, uint8_t
#include <map>         	// for map, operator==
#include <sstream>     	// for basic_ostringstream, basic_ostream, char_traits
#include <stdexcept>   	// for logic_error
#include <string>      	// for basic_string, allocator, string
#include <utility>     	// for pair

#include "Status.hpp"   // for Status, StatusType, operator==
#include "protocol.hpp"	// for PROTOCOL

/*******************************Constructors/Destructors*********************************/

std::string	buildPage(uint16_t code, const std::string& text);

Status::Status(uint16_t code, const std::string &text) :
	_code(code),
	_text(text),
	_representation(buildRepresentation()),
	_page(buildPage(code, text))
{
}

Status::Status(const Status& ref) :
	_code(ref._code),
	_text(ref._text),
	_representation(ref._representation),
	_page(ref._page)
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

/********************************Private Methods**********************************************/

std::string	buildPage(uint16_t code, const std::string& text)
{
	if (!Status::isCodeOfType(code, (StatusType)(STATUS_ERROR | STATUS_REDIRECTION)))
		return ("");

	std::ostringstream	oss;

	oss << 
"<html>\r\n\
<head><title>" << code << " " << text << "</title></head>\r\n\
<body>\r\n\
<center><h1>" << code << " " << text << "</h1></center>\r\n\
<hr><center>" SERVER_SOFTWARE "</center>\r\n\
</body>\r\n\
</html>\r\n";
	return (oss.str());
}

std::string	Status::buildRepresentation(void)
{
	std::ostringstream	oss;

	oss << PROTOCOL << " " << _code << " " << _text << "\r\n";
	return (oss.str());
}

/*******************************Getters/Setters****************************************/

uint16_t	Status::getCode(void) const
{
	return (_code);
}

const std::string&	Status::getRepresentation(void) const
{
	return (_representation);
}

size_t	Status::getRepresentationSize(void) const
{
	return (_representation.size());
}

const std::string&	Status::getText(void) const
{
	return (_text);
}

const std::string&	Status::getPage(void) const
{
	return (_page);
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
const Status*	Status::getStatus(int code)
{
	const std::map<int, Status>::const_iterator	it = Status::_statuses.find(code);
	if (it == _statuses.end())
		return (NULL);
	return (&it->second);
}
