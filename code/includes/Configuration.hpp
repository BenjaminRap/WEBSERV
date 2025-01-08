#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <vector>

# include "ServerConfiguration.hpp"

/// @brief This class describe all the configuration parsed from the configuration
/// file. This program should process multiples 'servers', each having their own
/// configuration
class Configuration : public std::vector<ServerConfiguration>
{
	public :

	Configuration(void);
	Configuration(std::string path);
	~Configuration(void);
}

#endif // !CONFIGURATION_HPP
