#include <bits/types/sig_atomic_t.h>
#include <csignal>

#include "socketCommunication.hpp"

/**
 * @brief A signal/thread safe variable that stores the received signal.
 * If no signal has been received, its value is NO_SIGNAL.
 */
volatile std::sig_atomic_t	signalStatus = NO_SIGNAL;

/**
 * @brief Set the signalStatus value.
 */
void	signalHandler(int signal)
{
	if (signalStatus == NO_SIGNAL)
		signalStatus = signal;
}

/**
 * @brief If a signal has been received, return it, otherwise, return NO_SIGNAL
 */
int	getSignalStatus()
{
	return (signalStatus);
}

/**
 * @brief Return the program exit status, depending on the signal status.
 */
int	getReturnCodeWithSignal()
{
	return (128 + signalStatus);
}
