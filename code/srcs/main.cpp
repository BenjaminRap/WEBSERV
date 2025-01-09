#include <iostream>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <cstdlib>
#include <csignal>

#include "socketCommunication.hpp"

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