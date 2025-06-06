#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

# include "ServerConfiguration.hpp"
# include "Host.hpp"

# define DEFAULT_CONFIG_PATH "./configurations/configuration.txt"

/**
 * @class Configuration
 *
 * @brief This class describe all the configuration parsed from the configuration
 * file. This program should process multiples 'servers', each having their own
 * configuration.
 * It is a map with the Host as key, and a vector of all ServerConfiguration as
 * value. Each host can have 1 or more ServerConfiguration.
 *
 */
class Configuration : public std::map<Host, std::vector<ServerConfiguration> >
{
private:
	/**
	 * @brief The boolean used to guarantee that there is only one instance.
	 * This variable is set to true when an instance is created and false
	 * when one is destroyed.
	 */
	static bool		_instanciated;

	/**
	 * @brief The max number of connection a socket can listen to.
	 */
	int				_maxConnectionBySocket;
	/**
	 * @brief The size of the _events array of the EPollHandler, it determine
	 * the max number of event the epoll_wait can put in the _events array.
	 */
	unsigned int	_maxEvents;
	/**
	 * @brief True if sockets can be reused just after being closed, false if
	 * there is a delay TIME_WAIT. It prevents the error bind() : address already
	 * in use but can cause other bugs. It should only be used in a production
	 * environment.
	 */
	bool			_reuseAddr;

	Configuration(void);
	Configuration(const Configuration& ref);
	Configuration&	operator=(const Configuration& ref);
public:
	Configuration(const char* path);
	~Configuration(void);
	
	int				getMaxConnectionBySocket(void) const;
	unsigned int	getMaxEvents(void) const;
	bool			getReuseAddr(void) const;
};

std::ostream & operator<<(std::ostream & o, Configuration const & rhs);

#endif // !CONFIGURATION_HPP
