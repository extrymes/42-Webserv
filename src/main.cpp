#include <iostream>
#include "colors.h"
#include "webserv.hpp"
#include "socket.hpp"

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << RED "Usage:\n\t./webserv <config_file>" RESET << std::endl;
		return 1;
	}
	std::signal(SIGPIPE, SIG_IGN);
	t_config config;
	t_socket socketConfig;
	try {
		Config::parseConfigFile(av[1], config);
		handleSocket(config, socketConfig);
		// for (std::vector<t_server>::iterator it = config.servers.begin(); it != config.servers.end(); ++it)
		// 	std::cout << GREEN << "Server name: " << it->name << std::endl;
	}
	catch (const std::exception& e) {
		std::cerr << RED "Error: " << e.what() << RESET << std::endl;
	}
	return 0;
}
