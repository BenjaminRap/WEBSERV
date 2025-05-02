#ifndef CGI_OUT_HPP
# define CGI_OUT_HPP

# include <cstdio>					// for L_tmpnam

# include "AFdData.hpp"				// for ASocketData
# include "FileFd.hpp"
# include "Headers.hpp"				// for Headers
# include "ServerConfiguration.hpp"	// for ServerConfiguration

class	FlowBuffer;
class	Status;

enum	CgiOutState
{
	READ_HEADER,
	CGI_TO_TEMP,
	WRITE_FIRST_PART,
	FILE_TO_BUFFER,
	CGI_TO_BUFFER,
	DONE
};

class CgiOut : public AFdData
{
private:
	FlowBuffer&					_flowBuf;
	std::string					_firstPart;
	size_t						_charsWritten;
	Headers						_headers;
	char						_tempName[L_tmpnam];
	FileFd*						_srcFile;
	CgiOutState					_state;
	uint16_t					_code;
	bool						_error;
	const ServerConfiguration&	_serverConf;

	CgiOut(void);
	CgiOut(const CgiOut &ref);

	CgiOut&			operator=(const CgiOut &ref);

	uint16_t	checkHeaders(void);
	uint16_t	getStatusCode(void);
	void		readHeaders(void);
	void		generateFirstPart(void);
	void		setErrorPage(const Status** currentStatus);
	void		handleClosingCgi(void);
	void		readFromCgi(void);
	void		writeToTemp(void);
	void		writeFirstPart(void);
	void		writeToBuff(void);
	void		setFinished(void);

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
