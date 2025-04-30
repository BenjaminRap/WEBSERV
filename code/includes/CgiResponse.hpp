#ifndef CGI_RESPONSE_HPP
# define CGI_RESPONSE_HPP

# include "ABody.hpp"	// for ABody
# include "Headers.hpp"	// for Headers

enum	CgiState
{
	READ_HEADER,
	CGI_TO_TEMP,
	TEMP_TO_FD,
	CGI_TO_FD
};

class CgiResponse : ABody
{
private:
	static const std::string	_lineEnd;

	std::string		_firstPart;
	size_t			_charsWritten;
	Headers			_headers;
	std::FILE*		_tempFile;
	CgiState		_state;
	
	CgiResponse();
	CgiResponse(const CgiResponse& ref);

	CgiResponse &operator=(const CgiResponse& ref);

	ssize_t		readHeader(const char* begin, const char* end);
	ssize_t		writeCgiResponseToFd(const char* begin, const char* end);
	uint16_t	checkHeaders(void);
	void		generateFirstPart(uint16_t code);
	ssize_t		writeFirstPart(void);
	ssize_t		writeCgiBody(const char* begin, const char* end);
public:
	CgiResponse(int fd);
	~CgiResponse();

	ssize_t			writeToFd(const void *buffer, size_t bufferCapacity);
};

#endif // !CGI_RESPONSE_HPP
