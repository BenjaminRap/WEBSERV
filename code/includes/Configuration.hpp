#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include <vector>

# include "ServerConfiguration.hpp"

class Configuration
{
private:
	std::vector<ServerConfiguration>	serverConfigurations;
};

#endif // !CONFIGURATION_HPP
