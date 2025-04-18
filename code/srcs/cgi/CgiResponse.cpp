#include <algorithm>				// for std::search
#include <stdexcept>				// for std::logic_error

#include "CgiResponse.hpp"			// for CgiResponse
#include "Status.hpp"				// for Status
#include "requestStatusCode.hpp"	// for HTTP_...

const std::string	CgiResponse::_lineEnd("\r\n");

CgiResponse::CgiResponse(int fd) :
	ABody(fd),
	_firstPart(),
	_charsWritten(0),
	_headers(),
	_areHeadersDone(false)
{
}

CgiResponse::~CgiResponse()
{
}


uint16_t	CgiResponse::checkHeaders(void)
{
	if (_headers.getHeader("content-type") == NULL)
		return (HTTP_INTERNAL_SERVER_ERROR);
	const std::string*	status = _headers.getHeader("status");
	if (status != NULL)
	{
		char	*end;
		const long	code = std::strtol(status->c_str(), &end, 10);

		_headers.erase("status");
		if (end != status->c_str() + 2 || code < 100 || code >= 600)
			return (HTTP_INTERNAL_SERVER_ERROR);
		return (code);
	}
	if (_headers.getHeader("Location") != NULL)
		return (HTTP_FOUND);
	return (HTTP_OK);
}


void	setFirstPart
(
	std::string& result,
	const Status& status,
	const std::string& autoIndexPage,
	const Headers& headers
);

void	CgiResponse::generateFirstPart(void)
{
	_areHeadersDone = true;
	const uint16_t	code = checkHeaders();
	try
	{
		const Status&	status = Status::getStatus(code);
		setFirstPart(_firstPart, status, "", _headers);
	}
	catch (std::logic_error& err)
	{
		const Status&	status = Status::getStatus(HTTP_INTERNAL_SERVER_ERROR);
		setFirstPart(_firstPart, status, "", _headers);
	}
}

ssize_t		CgiResponse::readHeader(const char* begin, const char* end)
{

	const char*  const	lineBreak = std::search(begin, end, _lineEnd.begin(), _lineEnd.end());

	if (lineBreak == end)
		return (0);
	if (lineBreak == begin)
	{
		generateFirstPart();
		return (_lineEnd.size());
	}
	const uint16_t	code = _headers.parseHeader(begin, lineBreak + 1);
	if (code != HTTP_OK)
	{
		setFinished(code);
		return (-1);
	}
	return (std::distance(begin, lineBreak + _lineEnd.size()));
}


ssize_t	CgiResponse::writeFirstPart(void)
{
	const size_t	numCharsToWrite = _firstPart.length() - _charsWritten;
	const ssize_t	written = writeOrIgnore(_firstPart.c_str() + _charsWritten, numCharsToWrite);

	if (written == -1)
		return (-1);
	_charsWritten += written;
	return (0);
}

ssize_t	CgiResponse::writeBody(const char* begin, const char* end)
{
	const	size_t	numCharsToWrite = std::distance(begin, end);

	return (writeOrIgnore(begin, numCharsToWrite));
}

ssize_t		CgiResponse::writeCgiResponseToFd(const char* begin, const char* end)
{
	ssize_t	totalConsumed = 0;

	while (_areHeadersDone == false)
	{
		ssize_t	consumed = readHeader(begin + totalConsumed, end);

		if (consumed == 0)
			return (totalConsumed);
		if (consumed == -1)
			return (-1);
		totalConsumed += consumed;
	}
	if (_charsWritten < _firstPart.length())
	{
		if (writeFirstPart() == -1)
			return (-1);
	}
	else
	{
		const ssize_t	consumed = writeBody(begin + totalConsumed, end);

		if (consumed == -1)
			return (-1);
		totalConsumed += consumed;
	}
	return (totalConsumed);
}

ssize_t		CgiResponse::writeToFd(const void *buffer, size_t bufferCapacity)
{
	char*	begin = (char*)buffer;
	char*	end = begin + bufferCapacity;

	return (writeCgiResponseToFd(begin, end));
}
