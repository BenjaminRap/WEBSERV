#include <bits/types/sig_atomic_t.h>
#include <csignal>

#include "socketCommunication.hpp"

/**
 * @brief A signal/thread safe variable that stores the received signal.
 * If no signal has been received, its value is NO_SIGNAL.
 */
volatile std::sig_atomic_t	signalStatus = NO_SIGNAL;

void	signalHandler(int signal)
{
	if (signalStatus == NO_SIGNAL)
		signalStatus = signal;
}

int	getSignalStatus()
{
	return (signalStatus);
}

int	getReturnCodeWithSignal()
{
	if (signalStatus == NO_SIGNAL)
		return (1);
	return (128 + signalStatus);
}

bool	addSignals(void)
{
	if (checkError(std::signal(SIGINT, signalHandler), SIG_ERR, "signal() : "))
		return (false);
	if (checkError(std::signal(SIGTERM, signalHandler), SIG_ERR, "signal() : "))
		return (false);
	if (checkError(std::signal(SIGPIPE, SIG_IGN), SIG_ERR, "signal() : "))
		return (false);
	return (true);
}
