#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "ASocketData.hpp"	// for ASocketData

class	ConnectedSocketData;
class	Response;

class	FlowBuffer;
class	ABody;

class CgiIn : public ASocketData
{
private:
	FlowBuffer&				_requestFlowBuffer;
	ABody&					_body;
	ConnectedSocketData&	_connectedSocketData;
	Response&				_response;

	CgiIn(void);
	CgiIn(const CgiIn &ref);

	CgiIn&	operator=(const CgiIn &ref);

public:
	CgiIn
	(
		int fd,
		SocketsHandler &socketsHandler,
		const std::vector<ServerConfiguration> &serverConfigurations,
		FlowBuffer& requestFlowBuffer,
		ABody&		body,
		ConnectedSocketData& connectedSocketData,
		Response& currentResponse
	);
	~CgiIn();

	void	callback(uint32_t events);
};

#endif // !CGI_IN_HPP
