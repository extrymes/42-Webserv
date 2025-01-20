#include "socket.hpp"
#include "webserv.hpp"

sockaddr_in init_sockaddr_in(t_config serverConfig) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(8080); // tmp
	return (server_addr);
}

void handleSocket(t_config serverConfig, t_socket &socketConfig)
{
	socketConfig.server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.server_fd < 0)
		throw std::runtime_error("socket fail");
	socketConfig.server_addr = init_sockaddr_in(serverConfig);
	if (bind(socketConfig.server_fd, (struct sockaddr*)&socketConfig.server_addr, sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("bind fail");
	if (listen(socketConfig.server_fd, 5) < 0)
		throw std::runtime_error("listen fail");
	handleSocketClient(socketConfig, serverConfig);
	socklen_t client_len = sizeof(socketConfig.client_addr);
	if (accept(socketConfig.server_fd,(struct sockaddr*)&socketConfig.client_addr, &client_len) < 0)
		throw std::runtime_error("accept fail");
}
