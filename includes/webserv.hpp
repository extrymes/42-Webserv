#pragma once
# include <iostream>
# include <string>
# include <fstream>
# include <sstream>
# include <exception>
# include <algorithm>
# include <fcntl.h>
# include <sstream>
# include <csignal>
# include "ParseConfig.hpp"
# include "socket.hpp"
# include "colors.h"
# include <map>

typedef struct s_socket t_socket;
typedef struct s_config t_config;

// socket
void handleSocket(t_config serverConfig, t_socket &socketConfig);
// void handleSocketClient(t_socket &socketConfig, t_config serverConfig);
