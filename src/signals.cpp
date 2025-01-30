#include "signals.hpp"

void handleSignals() {
	signal(SIGINT, sigintHandler);
}

void sigintHandler(int) {
	sigintReceived(true);
}

bool sigintReceived(bool newState) {
	static bool state = false;
	bool tmp = state;
	state = newState;
	return tmp;
}
