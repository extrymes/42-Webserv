#include "signal.hpp"

sig_atomic_t stopRequested = 0;

void handleSignal(int signal) {
	if (signal == SIGINT)
		stopRequested = 1;
}

void setupSignalHandler() {
	if (std::signal(SIGINT, handleSignal) == SIG_ERR)
		throw std::runtime_error("Failed to set up SIGINT signal handler");
}
