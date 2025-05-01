#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "AFdData.hpp"	// for AFdData
# include "FileFd.hpp"	// for FileFd
# include <cstdio>		// for L_tmpnam

class	ConnectedSocketData;
class	Response;

class	FlowBuffer;
class	ABody;

enum	CgiInState
{
	BUF_TO_TEMP,
	TEMP_TO_CGI,
	BUF_TO_CGI
};

class CgiIn : public AFdData
{
private:
	FlowBuffer&				_flowBuf;
	ABody&					_body;
	ConnectedSocketData&	_connectedSocketData;
	Response&				_response;
	char					_tempName[L_tmpnam];
	FileFd*					_tempFile;
	CgiInState				_state;

	CgiIn(void);
	CgiIn(const CgiIn &ref);

	CgiIn&	operator=(const CgiIn &ref);

public:
	CgiIn
	(
		int fd,
		EPollHandler& ePollHandler,
		FlowBuffer& requestFlowBuffer,
		ABody&		body,
		ConnectedSocketData& connectedSocketData,
		Response& currentResponse
	);
	~CgiIn();

	void	setFinished(uint16_t code);
	void	redirectToTemp(void);
	void	callback(uint32_t events);
};

#endif // !CGI_IN_HPP
