#pragma once
# include <iostream>
# include <fstream>
# include <sstream>
# include <exception>
# include "Config.hpp"
# include "colors.h"

// socket
void handleSocket(t_config serverConfig, t_socket &socketConfig);
void handleSocketClient(t_socket &socketConfig, t_config serverConfig);
