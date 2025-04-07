#include <cctype>					// for std::tolower
#include <algorithm>				// for std::find
#include <string>					// for std::string

#include "Request.hpp"				// for Request
#include "protocol.hpp"				// for PROTOCOL, PROTOCOL_LENGTH
#include "requestStatusCode.hpp"	// fpr HTTP_...

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
	if (std::distance(begin, end) != PROTOCOL_LENGTH)
		return (HTTP_BAD_REQUEST);
	if (std::memcmp(begin, PROTOCOL, PROTOCOL_LENGTH))
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

static char toLowerCase(char& c)
{
    return (std::tolower(c));
}

int		Request::parseHeader(const char *line, const char *end)
{
	if (std::distance(line, end) < 5)
		return (HTTP_BAD_REQUEST);
	const char * const keyEnd = std::find(line, end, ':');

	if (keyEnd == end || *(keyEnd + 1) != ' ')
		return (HTTP_BAD_REQUEST);

	const char * const valueEnd = end - 1;
	if (*valueEnd != '\r')
		return (HTTP_BAD_REQUEST);
	const std::string key(line, keyEnd);
	const char * valuePosition = keyEnd + 2;
	std::string value(valuePosition, valueEnd);
	std::transform(value.begin(), value.end(), value.begin(), toLowerCase);
	this->_headers[key] = value;
	return (HTTP_OK);
}
