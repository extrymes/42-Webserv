#include "signal.hpp"

sig_atomic_t stopRequested = 0;

void handleSignal(int signal) {
	if (signal == SIGINT)
		stopRequested = 1;
}

void setupSignalHandler() {
	struct sigaction sa;
	std::memset(&sa, 0, sizeof(sa));
	sa.sa_handler = handleSignal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	 if (sigaction(SIGINT, &sa, NULL) == -1)
		throw std::runtime_error("sigaction failed");
}
