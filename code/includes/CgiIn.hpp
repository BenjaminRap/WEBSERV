#ifndef CGI_IN_HPP
# define CGI_IN_HPP

# include "AFdData.hpp"	// for AFdData

class CgiIn : AFdData
{
private:
	CgiIn(void);
	CgiIn(const CgiIn &ref);

	CgiIn&	operator=(const CgiIn &ref);

public:
	CgiIn
	(
		int fd,
		SocketsHandler &socketsHandler,
		const std::vector<ServerConfiguration> &serverConfigurations
	);
	~CgiIn();

	void	callback(uint32_t events);
};

#endif // !CGI_IN_HPP
