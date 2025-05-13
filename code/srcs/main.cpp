#include <csignal>                  // for signal, SIG_ERR, SIGINT, SIGPIPE
#include <cstdlib>                  // for EXIT_FAILURE
#include <iostream>                 // for basic_ostream, operator<<, cerr

#include "Configuration.hpp"        // for Configuration, DEFAULT_CONFIG_PATH
#include "parsing.hpp"              // for ft_readfile, parse_file
#include "socketCommunication.hpp"  // for checkError, getReturnCodeWithSignal

int	main(int argc, char **argv)
{
	std::cout << "TODO : fix les error pages ignore en cas classique et dans le cgiResponse" << std::endl;
	if (argc == 1)
		std::cout << "No argument supplied, using the default path for the configuration." << std::endl;
	else if (argc > 2)
	{
		std::cerr << "Too much arguments supplied: webserv (configuration/path)?" << std::endl;
		return (EXIT_FAILURE);
	}
	if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : "))
		return (EXIT_FAILURE);
	if (checkError(std::signal(SIGTERM, signalHandler), SIG_ERR, "signal() : "))
		return (EXIT_FAILURE);
	if (checkError(std::signal(SIGPIPE, SIG_IGN), SIG_ERR, "signal() : "))
		return (EXIT_FAILURE);

	try
	{
		Configuration	conf((argc == 2) ? argv[1] : DEFAULT_CONFIG_PATH);
		while(getSignalStatus() == NO_SIGNAL)
		{
			handleIOEvents(conf);
		}
		return (getReturnCodeWithSignal());
	}
	catch (std::exception& exception)
	{
		std::cerr << exception.what() << std::endl;
		return (EXIT_FAILURE);
	}
}
