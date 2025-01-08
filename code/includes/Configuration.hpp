#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "ServerConfiguration.hpp"

/// @brief This class describe all the configuration parsed from the configuration
/// file. This program should process multiples 'servers', each having their own
/// configuration
class Configuration : public std::map<Host, std::vector<ServerConfiguration> >
{
public:
	int					maxConnectionBySocket;
	int					maxEvents;
	bool				reuseAddr;
};

#endif // !CONFIGURATION_HPP
