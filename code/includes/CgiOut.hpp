#ifndef CGI_OUT_HPP
# define CGI_OUT_HPP

# include "AFdData.hpp"	// for AFdData

class CgiOut : AFdData
{
private:
	CgiOut(void);
	CgiOut(const CgiOut &ref);

	CgiOut&	operator=(const CgiOut &ref);

public:
	CgiOut
	(
		int fd,
		SocketsHandler &socketsHandler,
		const std::vector<ServerConfiguration> &serverConfigurations
	);
	~CgiOut();

	void	callback(uint32_t events);
};

#endif // !CGI_OUT_HPP
