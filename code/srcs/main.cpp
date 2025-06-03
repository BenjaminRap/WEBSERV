#include <cstdlib>                  // for EXIT_FAILURE
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, char_traits, opera...

#include "Configuration.hpp"        // for Configuration, DEFAULT_CONFIG_PATH
#include "socketCommunication.hpp"  // for addSignals, getReturnCodeWithSignal

int	main(int argc, char **argv)
{
	if (argc == 1)
		std::cout << "No argument supplied, using the default path for the configuration." << std::endl;
	else if (argc > 2)
	{
		std::cerr << "Too much arguments supplied: webserv (configuration/path)?" << std::endl;
		return (EXIT_FAILURE);
	}
	if (!addSignals())
		return (EXIT_FAILURE);

	try
	{
		Configuration	conf((argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH);

		handleIOEvents(conf);
		return (getReturnCodeWithSignal());
	}
	catch (std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		return (EXIT_FAILURE);
	}
}
