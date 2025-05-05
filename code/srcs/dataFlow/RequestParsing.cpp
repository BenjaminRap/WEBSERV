#include <algorithm>              // for find
#include <cctype>                 // for isdigit
#include <cstring>                // for memcmp
#include <exception>              // for exception
#include <iterator>               // for distance
#include <string>                 // for basic_string, string

#include "EMethods.hpp"           // for getMethodFromBuffer
#include "Request.hpp"            // for Request
#include "protocol.hpp"           // for PROTOCOL_MAJOR, PROTOCOL_MINOR
#include "requestStatusCode.hpp"  // for HTTP_BAD_REQUEST, HTTP_OK, HTTP_HTT...

unsigned long	strToULongBase(const char *begin, const char* end, int (&isInBase)(int character), int base);

int	Request::parseMethod(const char *begin, const char *end)
{
	if (begin == end)
		return (HTTP_BAD_REQUEST);
	for (const char *index = begin; index != end; index++)
	{
		if (*index < 'A' || *index > 'Z')
			return (HTTP_BAD_REQUEST);
	}
	try {
		_statusLine.method = getMethodFromBuffer(begin, std::distance(begin, end));
	}
	catch (std::exception& exception) {
		return (HTTP_METHOD_NOT_ALLOWED);
	}
	return (HTTP_OK);
}

int	Request::parseProtocol(const char *begin, const char *end)
{
	const char*		index;

	if (std::memcmp(begin, "HTTP/", 5) == 0)
		index = begin + 5;
	else if (std::memcmp(begin, "HTTPS/", 6) == 0)
		index = begin + 6;
	else
		return (HTTP_BAD_REQUEST);
	const char*	delimiter = std::find(index, end, '.');
	if (delimiter == end)
		return (HTTP_BAD_REQUEST);
	const unsigned long	major = strToULongBase(index, delimiter, std::isdigit, 10);
	if (major == (unsigned long)-1)
		return (HTTP_BAD_REQUEST);
	const unsigned long	minor = strToULongBase(delimiter + 1, end, std::isdigit, 10);
	if (minor == (unsigned long)-1)
		return (HTTP_BAD_REQUEST);
	if (major != PROTOCOL_MAJOR || minor != PROTOCOL_MINOR)
		return (HTTP_HTTP_VERSION_NOT_SUPPORTED);
	return (HTTP_OK);
}

int		Request::parseStatusLine(const char *line, const char *end)
{
	//Parsing the method
	const char	*methodEnd = std::find(line, end, ' ');
	{
		if (methodEnd == end)
			return (HTTP_BAD_REQUEST);
		const int code = parseMethod(line, methodEnd);
		if (code != HTTP_OK)
			return (code);
	}

	//Parsing the target
	const char	*targetEnd = std::find(methodEnd + 1, end, ' ');
	if (targetEnd == end)
		return (HTTP_BAD_REQUEST);
	_statusLine.requestTarget = std::string(methodEnd + 1, targetEnd);

	//Parsing the protocol
	const char	*protocolEnd = std::find(targetEnd + 1, end, '\r');
	{
		if (protocolEnd == end)
			return (HTTP_BAD_REQUEST);
		const int code = parseProtocol(targetEnd + 1, protocolEnd);
		if (code != HTTP_OK)
			return (code);
	}
	return (HTTP_OK);
}
