#include <iostream>
#include "webserv.hpp"
#include "socket.hpp"

int main(int ac, char **av) {
	if (ac != 2) {
		std::cerr << "Usage:\n\t./webserv <config_file>" << std::endl;
		return 1;
	}
	std::signal(SIGPIPE, SIG_IGN);
	std::vector<t_server> servers;
	t_socket socketConfig;
	try {
		ParseConfig(av[1], servers);
		handleSocket(servers, socketConfig);
	} catch (const std::exception& e) {
		std::cerr << "webserv: error: " << e.what() << std::endl;
	}
	return 0;
}
