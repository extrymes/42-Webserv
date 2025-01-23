#include "socket.hpp"
#include "webserv.hpp"

std::string checkExt(std::string file) {
	const char *ext = strrchr(file.c_str(), '.');
	if (!ext)
		return "text/html";
	std::string str = ext;
	if (str == ".js")
		return "application/javascript";
	else if (str == ".css")
		return "text/css";
	else if (str == ".html")
		return "text/html";
	else
		return "text/plain";
}

int handlePollout(t_socket &socketConfig, struct pollfd *clients, int i, t_config serverConfig) {
	// (void)serverConfig;
	std::string root = "./web";
	std::string index = "/test.html";
	std::string	file;
	if (socketConfig.buffClient.url == "/")
		file = root + index;
	else
		file = root + socketConfig.buffClient.url;
	std::cout << socketConfig.buffClient.url << std::endl;
	std::string finalFile = readHtml(file, serverConfig, checkExt(file));
	std::cout << finalFile.c_str() << std::endl;
	if (send(clients[i].fd, finalFile.c_str(), finalFile.size(), 0) < 0) {
		perror(NULL);
		handleDeconnexionClient(i, clients);
		return -1;
	}
	return 0;
}

int	handlePollin(t_socket &socketConfig, struct pollfd *clients, int i, int &client_count) {
	if (clients[i].fd == socketConfig.server_fd) {
		socketConfig.client_len = sizeof(socketConfig.client_addr);
		checkEmptyPlace(socketConfig, clients);
		if (client_count <= MAX_CLIENTS)
			client_count++;
	}
	else {
		char buffer[4096];
		if (recv(clients[i].fd, buffer, sizeof(buffer), 0) < 0) {
			handleDeconnexionClient(i, clients);
			return -1;
		}
		parseBuffer(buffer, socketConfig.buffClient);
	}
	return 0;
}

void	initSocket(t_socket &socketConfig, t_config serverConfig, struct pollfd *clients) {
	socketConfig.server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.server_fd < 0)
		throw std::runtime_error("socket fail");
	socketConfig.server_addr = init_sockaddr_in(serverConfig);
	if (bind(socketConfig.server_fd, (const struct sockaddr *)&socketConfig.server_addr, sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("bind fail");
	if (listen(socketConfig.server_fd, 5) < 0)
		throw std::runtime_error("listen fail");
	clients[0].fd = socketConfig.server_fd;
	clients[0].events = POLLIN | POLLOUT;
}

void handleSocket(t_config serverConfig, t_socket &socketConfig) {
	int	client_count = 1;
	struct pollfd	clients[MAX_CLIENTS]; //Create struct
	memset(clients, 0, sizeof(clients));
	initSocket(socketConfig, serverConfig, clients);
	while (1) {
		if (poll(clients, client_count, -1) < 0)
			throw std::runtime_error("poll failed");
		for (int i = 0; i < client_count; i++) {
			if (clients[i].revents & POLLIN) {
				if (handlePollin(socketConfig, clients, i, client_count) == -1)
					continue ;
			}
			if (clients[i].revents & POLLOUT) {
				if (handlePollout(socketConfig, clients, i, serverConfig) == -1)
					continue ;
			}
		}
	}
	std::cout << "test8" << std::endl;
	// close(socketConfig.server_fd);
}

