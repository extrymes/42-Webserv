#include "socket.hpp"
#include "webserv.hpp"

void handleSocketClient(t_socket &socketConfig, t_config serverConfig) {
	socketConfig.client_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.client_fd < 0)
		throw std::runtime_error("socket fail");
	if (connect(socketConfig.client_fd,(struct sockaddr*)&socketConfig.server_addr,sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("connection socket failed");
	socketConfig.client_addr = init_sockaddr_in(serverConfig);
}
