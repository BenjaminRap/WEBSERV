#include <csignal>

#include "socketCommunication.hpp"

volatile std::sig_atomic_t	signalStatus = NO_SIGNAL;

void	signalHandler(int signal)
{
	signalStatus = signal;
}

int	getSignalStatus()
{
	return (signalStatus);
}