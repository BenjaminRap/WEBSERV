#ifndef CGI_OUT_HPP
# define CGI_OUT_HPP

# include "AFdData.hpp"	// for AFdData

class	ABody;
class	FlowBuffer;

class CgiOut : AFdData
{
private:
	FlowBuffer&	_responseFlowBuffer;

	CgiOut(void);
	CgiOut(const CgiOut &ref);

	CgiOut&	operator=(const CgiOut &ref);

public:
	CgiOut
	(
		int fd,
		SocketsHandler &socketsHandler,
		const std::vector<ServerConfiguration> &serverConfigurations,
		FlowBuffer& responseFlowBuffer
	);
	~CgiOut();

	void	callback(uint32_t events);
};

#endif // !CGI_OUT_HPP
