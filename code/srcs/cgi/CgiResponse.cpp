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
	_tempFile(NULL),
	_state(READ_HEADER),
	_code(0)
{
}

CgiResponse::~CgiResponse()
{
	if (_tempFile != NULL)
		fclose(_tempFile);
}

unsigned long	stringToULongBase(const std::string& str, int (&isInBase)(int character), int base);

uint16_t	CgiResponse::checkHeaders(void)
{
	if (_headers.getHeader("content-type") == NULL)
		return (HTTP_BAD_GATEWAY);
	const std::string*	contentLength = _headers.getHeader("content-length");
	const std::string*	transferEncoding = _headers.getHeader("transfer-encoding");
	if (contentLength != NULL && transferEncoding != NULL)
		return (HTTP_BAD_GATEWAY);
	if (contentLength != NULL)
	{
		const unsigned long	size = stringToULongBase(*contentLength, std::isdigit, 10);
		if (size == (unsigned long)-1)
			return (HTTP_BAD_GATEWAY);
		_state = CGI_TO_FD;
	}
	else if (transferEncoding == NULL || *transferEncoding != "chunked")
		_state = CGI_TO_FD;
	else
	{
		_tempFile = std::tmpfile();
		if (_tempFile == NULL)
			return (HTTP_INTERNAL_SERVER_ERROR);
		_state = CGI_TO_TEMP;
	}

	const std::string*	status = _headers.getHeader("status");
	if (status != NULL)
	{
		char	*end;
		const unsigned long	code = std::strtoul(status->c_str(), &end, 10);

		_headers.erase("status");
		if (end != status->c_str() + 2 || code < 100 || code >= 600)
			return (HTTP_BAD_GATEWAY);
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
	const Headers& headers,
	bool hasBody
);

void	CgiResponse::generateFirstPart(void)
{
	try
	{
		const Status&	status = Status::getStatus(_code);
		setFirstPart(_firstPart, status, "", _headers, true);
	}
	catch (std::logic_error& err)
	{
		const Status&	status = Status::getStatus(HTTP_BAD_GATEWAY);
		setFirstPart(_firstPart, status, "", _headers, false);
	}
}

ssize_t		CgiResponse::readHeader(const char* begin, const char* end)
{

	const char*  const	lineBreak = std::search(begin, end, _lineEnd.begin(), _lineEnd.end());

	if (lineBreak == end)
		return (0);
	if (lineBreak == begin)
	{
		_code = checkHeaders();

		if (_state != CGI_TO_TEMP)
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

ssize_t	CgiResponse::writeCgiBody(const char* begin, const char* end)
{
	const	size_t	numCharsToWrite = std::distance(begin, end);

	if (_state == CGI_TO_TEMP)
	{
		const size_t	consumed = fwrite(begin, 1, numCharsToWrite, _tempFile);

		if (consumed < numCharsToWrite)
			return (-1);
		return (consumed);
	}
	else
		return (writeOrIgnore(begin, numCharsToWrite));
}

ssize_t		CgiResponse::writeCgiResponseToFd(const char* begin, const char* end)
{
	ssize_t	totalConsumed = 0;

	while (_state == READ_HEADER)
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
		const ssize_t	consumed = writeCgiBody(begin + totalConsumed, end);

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
