#ifndef CGI_OUT_HPP
# define CGI_OUT_HPP

# include "ASocketData.hpp"	// for ASocketData

class	ABody;
class	FlowBuffer;

class CgiOut : public ASocketData
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
