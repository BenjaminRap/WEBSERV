#include <algorithm>				// for std::find
#include <iostream>					// for operator<<

#include "Headers.hpp"				// for Headers
#include "requestStatusCode.hpp"	// for HTTP_...

/*******************************Constructors / Destructors**********************/

Headers::Headers(void)
{
}

Headers::~Headers(void)
{
}

/****************************Public Methods*************************************/

const std::string*	Headers::getHeader(const std::string &key) const
{
	Headers::const_iterator it = find(key);

	if (it != end())
		return (&it->second);
	return (NULL);
}

static char toLowerCase(char& c)
{
    return (std::tolower(c));
}

int	Headers::parseHeader(const char *line, const char *end)
{
	if (this->size() >= MAX_HEADER_COUNT)
		return (HTTP_BAD_REQUEST);
	if (std::distance(line, end) < 5)
		return (HTTP_BAD_REQUEST);
	const char * const keyEnd = std::find(line, end, ':');

	if (keyEnd == end || *(keyEnd + 1) != ' ')
		return (HTTP_BAD_REQUEST);

	const char * const valueEnd = end - 1;
	if (*valueEnd != '\r')
		return (HTTP_BAD_REQUEST);
	std::string key(line, keyEnd);
	std::transform(key.begin(), key.end(), key.begin(), toLowerCase);

	const char * valuePosition = keyEnd + 2;
	const std::string value(valuePosition, valueEnd);
	operator[](key) = value;
	return (HTTP_OK);
}


size_t	Headers::getTotalSize(void) const
{
	size_t	length = 0;

	for (Headers::const_iterator it = begin(); it != end(); it++)
	{
		length += it->first.size() + it->second.size();
		length += 2; // for the ": "
		length += LINE_END_LENGTH;
	}
	return (length);
}

/**************************Operator Overload*************************************/

std::string&	operator+=(std::string& dest, const Headers& headers)
{
	const size_t	length = headers.getTotalSize();

	dest.reserve(dest.length() + length + 1);
	for (Headers::const_iterator it = headers.begin(); it != headers.end(); it++)
	{
		dest.append(it->first);
		dest.append(": ");
		dest.append(it->second);
		dest.append(LINE_END);
	}
	return (dest);
}

std::ostream& operator<<(std::ostream& o, const Headers& headers)
{
	std::string	representation;

	representation += headers;
	o << representation;
	return (o);
}
