#include <iostream>
#include "colors.h"
#include "webserv.hpp"
#include "socket.hpp"

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << RED "Usage:\n\t./webserv <config_file>" RESET << std::endl;
		return 1;
	}
	t_config serverConfig;
	t_socket socketConfig;
	try {
		readFile(av[1]);
		handleSocket(serverConfig, socketConfig);
	}
	catch(const std::exception& e) {
		std::cerr << RED "Error: " << e.what() << RESET << std::endl;
	}
	return 0;
}
