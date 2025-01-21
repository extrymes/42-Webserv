#pragma once
# include <iostream>
# include <fstream>
# include <sstream>
# include <exception>
# include "colors.h"

// --- Structures ---
typedef struct data data;
typedef struct s_config t_config;

struct s_config {
	std::string serverHost;
	std::string serverName;
	int serverPort;
	std::string *errorPages;
	int maxClientBodySize;
};


// --- Parsing ---
// Reader
void	readFile(std::string filename);

// Utils
void	trim(std::string &str);

// socket
void	handleSocket(t_config serverConfig, t_socket &socketConfig);
void	handleSocketClient(t_socket &socketConfig, t_config serverConfig);
