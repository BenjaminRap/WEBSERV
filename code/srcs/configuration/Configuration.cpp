#include <stddef.h>                 // for size_t
#include <iostream>                 // for basic_ostream, operator<<, cout
#include <map>                      // for _Rb_tree_const_iterator, operator!=
#include <stdexcept>                // for logic_error
#include <string>                   // for char_traits, basic_string
#include <utility>                  // for pair
#include <vector>                   // for vector

#include "Configuration.hpp"        // for Configuration, operator<<
#include "Host.hpp"                 // for Host
#include "ServerConfiguration.hpp"  // for operator<<, ServerConfiguration

bool	Configuration::_instanciated = false;

Configuration::Configuration(void)
	: _maxConnectionBySocket(10000), _maxEvents(10000), _reuseAddr(true)
{
	if (_instanciated)
		throw std::logic_error("Error : Trying to instanciate a Configuration multiples times");
	Configuration::_instanciated = true;
}

Configuration::~Configuration(void)
{
	Configuration::_instanciated = false;
}

int	Configuration::getMaxConnectionBySocket(void) const
{
	return (_maxConnectionBySocket);
}

unsigned int	Configuration::getMaxEvents(void) const
{
	return (_maxEvents);
}

bool	Configuration::getReuseAddr(void) const
{
	return (_reuseAddr);
}

std::ostream & operator<<(std::ostream & o, Configuration const & rhs)
{
	size_t	i = 1;

	for (std::map<Host, std::vector<ServerConfiguration> >::const_iterator it = rhs.begin(); it != rhs.end(); ++it)
	{
		std::cout << "Host " << i << ":" << std::endl;
		for (size_t j = 0; j < it->second.size(); j++)
		{
			std::cout << "Server " << j + 1 << ":" << std::endl;
			std::cout << it->second[j] << std::endl; 
		}
		std::cout << std::endl;
		i++;
	}
	return (o);
}
