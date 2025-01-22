#include "socket.hpp"
#include "webserv.hpp"

void handleSocketClient(t_socket &socketConfig, std::vector<t_server> servers) {
	socketConfig.client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.client_fd < 0)
		throw std::runtime_error("socket fail");
	socketConfig.client_addr = init_sockaddr_in(servers);
	// makeSocketNonBlocking(socketConfig.client_fd);
	// std::cout << "test = " << test << std::endl; // tmp
	if (connect(socketConfig.client_fd,(struct sockaddr*)&socketConfig.server_addr,sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("connection socket failed");
}
