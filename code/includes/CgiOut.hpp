#ifndef CGI_OUT_HPP
# define CGI_OUT_HPP

# include "AFdData.hpp"				// for ASocketData
# include "Headers.hpp"				// for Headers
# include "ServerConfiguration.hpp"	// for ServerConfiguration
# include <cstdio>					// for std::FILE

class	FlowBuffer;
class	Status;

enum	CgiOutState
{
	READ_HEADER,
	CGI_TO_FILE,
	FILE_TO_BUFFER,
	CGI_TO_BUFFER
};

class CgiOut : public AFdData
{
private:
	FlowBuffer&					_flowBuf;
	std::string					_firstPart;
	size_t						_charsWritten;
	Headers						_headers;
	std::FILE*					_srcFile;
	CgiOutState					_state;
	uint16_t					_code;
	bool						_error;
	const ServerConfiguration&	_serverConf;

	CgiOut(void);
	CgiOut(const CgiOut &ref);

	CgiOut&			operator=(const CgiOut &ref);

	uint16_t		checkHeaders(void);
	uint16_t		getStatusCode(void);
	ssize_t			readHeader(void);
	void			generateFirstPart(void);
	const Status*	setErrorPage(const Status* currentStatus);

public:
	CgiOut
	(
		int fd,
		EPollHandler& ePollHandler,
		FlowBuffer& responseFlowBuffer,
		const ServerConfiguration& serverConfiguration
	);
	~CgiOut();

	void	callback(uint32_t events);
};

#endif // !CGI_OUT_HPP
