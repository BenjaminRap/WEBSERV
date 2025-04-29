#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "AFdData.hpp"	// for AFdData
# include <cstdio>		// for FILE

class	ConnectedSocketData;
class	Response;

class	FlowBuffer;
class	ABody;

class CgiIn : public AFdData
{
private:
	FlowBuffer&				_requestFlowBuffer;
	ABody&					_body;
	ConnectedSocketData&	_connectedSocketData;
	Response&				_response;
	std::FILE*				_tempFile;
	ssize_t					_tempFileSize;

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

	void	callback(uint32_t events);
};

#endif // !CGI_IN_HPP
