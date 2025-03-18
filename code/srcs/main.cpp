#include <csignal>                  // for signal, SIG_ERR, SIGINT, SIGPIPE
#include <cstdlib>                  // for EXIT_FAILURE
#include <exception>                // for exception
#include <iostream>                 // for basic_ostream, operator<<, cerr
#include <string>                   // for char_traits, basic_string, string

#include "Configuration.hpp"        // for Configuration, DEFAULT_CONFIG_PATH
#include "parsing.hpp"              // for ft_readfile, parse_file
#include "socketCommunication.hpp"  // for checkError, getReturnCodeWithSignal

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
		if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : ") == SIG_ERR)
			return (EXIT_FAILURE);
		if (checkError(std::signal(SIGPIPE, SIG_IGN), SIG_ERR, "signal() : ") == SIG_ERR)
			return (EXIT_FAILURE);

		Configuration	conf;
		std::string		file;

		if (argc == 2)
			readfile(argv[1], file);
		else
			readfile(DEFAULT_CONFIG_PATH, file);
		parseFile(conf, file);
		while(getSignalStatus() == NO_SIGNAL)
		{
			try
			{
				handleIOEvents(conf);
			}
			catch(const std::exception& e)
			{
				std::cerr << e.what() << '\n';
			}
		}
		return (getReturnCodeWithSignal());
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
}
