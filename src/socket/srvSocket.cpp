#include "socket.hpp"
#include "webserv.hpp"

int	handlePollin(t_socket &socketConfig, struct pollfd *clients, int i, int &client_count) {
	if (clients[i].fd == socketConfig.server_fd) {
		socketConfig.client_len = sizeof(socketConfig.client_addr);
		checkEmptyPlace(socketConfig, clients);
		if (client_count <= MAX_CLIENTS)
			client_count++;
	}
	else {
		char buffer[1024];
		if (recv(clients[i].fd, buffer, sizeof(buffer), 0) < 0) {
			handleDeconnexionClient(i, clients);
			return -1;
		}
		// std::cout << "buff = " << buffer << std::endl;
		t_info_client buffClient;
		parseBuffer(buffer, buffClient);
	}
	return 0;
}

// void	initSocket() {

// }

void handleSocket(t_config serverConfig, t_socket &socketConfig) {
	std::string	index = "./web/test.html";
	int	client_count = 1;
	struct pollfd	clients[MAX_CLIENTS]; //Create struct

	socketConfig.server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.server_fd < 0)
		throw std::runtime_error("socket fail");
	socketConfig.server_addr = init_sockaddr_in(serverConfig);
	if (bind(socketConfig.server_fd, (const struct sockaddr *)&socketConfig.server_addr, sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("bind fail");
	if (listen(socketConfig.server_fd, 5) < 0)
		throw std::runtime_error("listen fail");
	memset(clients, 0, sizeof(clients));
	clients[0].fd = socketConfig.server_fd;
	clients[0].events = POLLIN | POLLOUT;
	while(1) {
		if (poll(clients, client_count, -1) < 0)
			throw std::runtime_error("poll failed");
		for (int i = 0; i < client_count; i++) {
			if (clients[i].revents & POLLIN) {
				if (handlePollin(socketConfig, clients, i, client_count) == -1)
					continue ;
			}
			if (clients[i].revents & POLLOUT) {
				std::string finalFile = readHtml(index);
				if (send(clients[i].fd, finalFile.c_str(), finalFile.size(), 0) < 0) {
					handleDeconnexionClient(i, clients);
					continue ;
				}
			}
		}
	}
	std::cout << "test8" << std::endl;
	// close(socketConfig.server_fd);
}

