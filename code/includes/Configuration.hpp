#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "ServerConfiguration.hpp"
# include "Host.hpp"

/// @brief This class describe all the configuration parsed from the configuration
/// file. This program should process multiples 'servers', each having their own
/// configuration
class Configuration : public std::map<Host, std::vector<ServerConfiguration> >
{
private:
	static bool		_instanciated;

	int				_maxConnectionBySocket;
	unsigned int	_maxEvents;
	bool			_reuseAddr;

	Configuration(const Configuration& ref);

	Configuration&	operator=(const Configuration& ref);	
public:
	Configuration(void);
	~Configuration(void);
	
	int				getMaxConnectionBySocket(void) const;
	unsigned int	getMaxEvents(void) const;
	bool			getReuseAddr(void) const;
};

#endif // !CONFIGURATION_HPP
