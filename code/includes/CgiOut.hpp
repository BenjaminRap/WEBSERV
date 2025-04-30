#ifndef CGI_OUT_HPP
# define CGI_OUT_HPP

# include "AFdData.hpp"	// for ASocketData
# include "Headers.hpp"	// for Headers
# include <cstdio>		// for std::FILE

class	FlowBuffer;

enum	CgiOutState
{
	READ_HEADER,
	CGI_TO_TEMP,
	TEMP_TO_BUFFER,
	CGI_TO_BUFFER
};

class CgiOut : public AFdData
{
private:
	FlowBuffer&	_flowBuf;
	std::string	_firstPart;
	size_t		_charsWritten;
	Headers		_headers;
	std::FILE*	_tempFile;
	CgiOutState	_state;
	uint16_t	_code;

	CgiOut(void);
	CgiOut(const CgiOut &ref);

	CgiOut&		operator=(const CgiOut &ref);

	ssize_t		readHeader(void);
	uint16_t	checkHeaders(void);
	void		generateFirstPart(void);

public:
	CgiOut
	(
		int fd,
		EPollHandler& ePollHandler,
		FlowBuffer& responseFlowBuffer
	);
	~CgiOut();

	void	callback(uint32_t events);
};

#endif // !CGI_OUT_HPP
