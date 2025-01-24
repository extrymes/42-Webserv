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
	std::vector<t_server> servers;
	t_socket socketConfig;
	try {
		ParseConfig(av[1], servers);
		for (std::vector<t_server>::iterator it = servers.begin(); it != servers.end(); ++it)
			std::cout << GREEN << "Server name: " << it->name << std::endl;
	} catch (const std::exception& e) {
		std::cerr << "webserv: [config] " << e.what() << std::endl;
	}
	try {
		handleSocket(servers, socketConfig);
	} catch (const std::exception& e) {
		std::cerr << RED "Error: " << e.what() << RESET << std::endl;
	}
	return 0;
}
