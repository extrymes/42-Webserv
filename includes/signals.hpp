#pragma once
# include <signal.h>

void handleSignals();
void sigintHandler(int sig);
bool sigintReceived(bool newState);
