#pragma once
# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <exception>
# include <algorithm>
# include <fcntl.h>
# include <sstream>
# include "Config.hpp"
# include "socket.hpp"
# include "colors.h"

// socket
void handleSocket(t_config serverConfig, t_socket &socketConfig);
void handleSocketClient(t_socket &socketConfig, t_config serverConfig);
