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

/**************************Operator Overload*************************************/


std::ostream& operator<<(std::ostream& o, const Headers& headers)
{
	Headers::const_iterator	it;

	for (it = headers.begin(); it != headers.end(); it++)
	{
		o << it->first << ": " << it->second << '\n';
	}
	return (o);
}
