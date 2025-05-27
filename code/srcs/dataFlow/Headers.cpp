#include <stddef.h>               // for size_t, NULL
#include <algorithm>              // for find, transform
#include <cctype>                 // for tolower
#include <iostream>               // for ostream, basic_ostream
#include <iterator>               // for distance
#include <map>                    // for operator!=, _Rb_tree_const_iterator
#include <string>                 // for basic_string, string, operator<<
#include <utility>                // for pair

#include "Headers.hpp"            // for Headers, LINE_END, LINE_END_LENGTH
#include "requestStatusCode.hpp"  // for HTTP_BAD_REQUEST, HTTP_OK

/*******************************Constructors / Destructors**********************/

Headers::Headers(void)
{
}

Headers::~Headers(void)
{
}

/****************************Public Methods*************************************/


InsertType	Headers::getHeaderInsertType(const std::string& header)
{
	const std::map<const std::string, InsertType>::const_iterator	type = _headersInsertType.find(header);

	if (type == _headersInsertType.end())
		return (REPLACE);
	return (type->second);
}

const std::string*	Headers::getUniqueHeader(const std::string &key) const
{
	const Headers::const_iterator it = find(key);

	if (it != end())
	{
		if (it->second.size() != 1)
			throw std::logic_error("getUniqueHeader called with a header with multiple values !");
		return (&it->second.front());
	}
	return (NULL);
}

bool	Headers::addHeader(const std::string&key, const std::string& value)
{
	const InsertType							type = Headers::getHeaderInsertType(key);
	const std::pair<Headers::iterator, bool>	elem = this->insert(std::make_pair(key, std::list<std::string>()));
	const bool									canInsert = elem.second;
	std::list<std::string>&						values = elem.first->second;

	if (canInsert)
	{
		values.push_back(value);
		return (true);
	}
	if (type == UNIQUE)
		return (false);
	std::string&	firstValue = values.front();

	if (type == CONCAT_COMMA || type == CONCAT_SEMICOLON)
	{
		const char*	separator = (type == CONCAT_COMMA) ? ", " : "; ";

		firstValue.reserve(firstValue.size() + value.size() + 3);
		firstValue.append(separator);
		firstValue.append(value);
	}
	else if (type == MULTIPLE)
		values.push_back(value);
	else if (type == REPLACE)
		firstValue = value;
	return (true);
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
	if (!addHeader(key, value))
		return (HTTP_BAD_REQUEST);
	return (HTTP_OK);
}


size_t	Headers::getTotalSize(void) const
{
	size_t	length = 0;

	for (Headers::const_iterator headerIt = begin(); headerIt != end(); headerIt++)
	{
		const std::list<std::string>&	values = headerIt->second;
		std::list<std::string>::const_iterator valueIt;

		for (valueIt = values.begin(); valueIt != values.end(); valueIt++)
		{
			length += headerIt->first.size();
			length += 2;
			length += valueIt->size();
			length += LINE_END_LENGTH;
		}
	}
	return (length);
}

/**************************Operator Overload*************************************/

std::string&	operator+=(std::string& dest, const Headers& headers)
{
	const size_t	length = headers.getTotalSize();

	dest.reserve(dest.length() + length + 1);
	for (Headers::const_iterator headerIt = headers.begin(); headerIt != headers.end(); headerIt++)
	{
		const std::list<std::string>&	values = headerIt->second;
		std::list<std::string>::const_iterator valueIt;

		for (valueIt = values.begin(); valueIt != values.end(); valueIt++)
		{
			dest.append(headerIt->first);
			dest.append(": ");
			dest.append(*valueIt);
			dest.append(LINE_END);
		}
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
