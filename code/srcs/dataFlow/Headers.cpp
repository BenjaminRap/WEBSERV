#include "Headers.hpp"				// for Headers
#include "requestStatusCode.hpp"	// for HTTP_...
#include <algorithm>				// for std::find

/*******************************Constructors / Destructors**********************/

Headers::Headers(void) :
	_headers()
{
}

Headers::~Headers(void)
{
}

/****************************Public Methods*************************************/

const std::string*	Headers::getHeader(const std::string &key) const
{
	std::map<std::string, std::string>::const_iterator it = _headers.find(key);

	if (it != _headers.end())
		return (&it->second);
	return (NULL);
}

const HeaderMapType	Headers::getMap(void) const
{
	return (_headers);
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
	_headers[key] = value;
	return (HTTP_OK);
}

/**************************Operator Overload*************************************/


std::ostream& operator<<(std::ostream& o, const Headers& headers)
{
	const HeaderMapType			internalHeaders = headers.getMap();

	HeaderMapType::const_iterator	it;

	for (it = internalHeaders.begin(); it != internalHeaders.end(); it++)
	{
		o << it->first << ": " << it->second << '\n';
	}
	return (o);
}
