#include <csignal>

#include "socketCommunication.hpp"

/**
 * @brief A signal/thread safe variable that store the received signal.
 * If no signal has been received, its value is NO_SIGNAL
 */
volatile std::sig_atomic_t	signalStatus = NO_SIGNAL;

/**
 * @brief Set the signalStatus value
 */
void	signalHandler(int signal)
{
	signalStatus = signal;
}

/**
 * @brief If a signal ha sbeen received, return it, otherwise, return NO_SIGNAL
 */
int	getSignalStatus()
{
	return (signalStatus);
}