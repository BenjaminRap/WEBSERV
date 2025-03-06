#include "Configuration.hpp"
#include "parsing.hpp"
#include <csignal>                  // for signal, SIG_ERR, SIGINT
#include <cstdlib>                  // for EXIT_FAILURE
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...
#include <string>                   // for basic_string
#include "Configuration.hpp"        // for Configuration
#include "socketCommunication.hpp"  // for getReturnCodeWithSignal, getSigna...

int	main(int argc, char **argv)
{
	if (argc == 1)
		std::cout << "No argument supplied, using the default path for the configuration." << std::endl;
	else if (argc > 2)
	{
		std::cout << "Too much arguments supplied: webserv (configuration/path)?" << std::endl;
	return (EXIT_FAILURE);
	}
	try
	{
		// if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : ") == SIG_ERR)
		// 	return (EXIT_FAILURE);

		Configuration	conf;
		std::string		file;

		if (argc == 2)
			ft_readfile(argv[1], file);
		else
			ft_readfile(DEFAULT_CONFIG_PATH, file);
		parse_file(conf, file);
		while(getSignalStatus() == NO_SIGNAL)
		{
			try
			{
				handleIOEvents(conf);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << std::endl;
			}
		}
		return (getReturnCodeWithSignal());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
