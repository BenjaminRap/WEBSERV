#include <csignal>                  // for signal, SIG_ERR, SIGINT
#include <cstdlib>                  // for EXIT_FAILURE
#include <exception>                // for exception
#include <iostream>                 // for char_traits, basic_ostream, opera...
#include <string>                   // for basic_string

#include "Configuration.hpp"        // for Configuration
#include "socketCommunication.hpp"  // for getReturnCodeWithSignal, getSigna...

/**
 * @brief Create a mostly uninitialize Configuration and call the function to
 * handle write and read request. It call the handleIoEvents till he receive a signal.
 * @return Return the signal + 128
 */
int	main(void)
{
	if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : ") == SIG_ERR)
		return (EXIT_FAILURE);

	Configuration	conf;

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