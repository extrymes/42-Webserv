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
	(void)serverConfig;

	if (send(clients[i].fd, socketConfig.buffClient.responseServer.c_str(), socketConfig.buffClient.responseServer.size(), 0) < 0) {
		handleDeconnexionClient(i, clients);
		return -1;
	}
	clients[i].events = POLLIN;
	return 0;
}

int	handlePollin(t_socket &socketConfig, struct pollfd *clients, int i, int &client_count, t_config serverConfig) {
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
		std::string root = "./web";
		std::string index = "/test.html"; // tmp
		std::string	file;
		if (socketConfig.buffClient.url == "/")
			file = root + index;
		else
			file = root + socketConfig.buffClient.url;
		socketConfig.buffClient.responseServer = readHtml(file, serverConfig, checkExt(file));
		clients[i].events = POLLIN | POLLOUT;
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
				if (handlePollin(socketConfig, clients, i, client_count, serverConfig) == -1)
					continue ;
			}
			if (clients[i].revents & POLLOUT) {
				if (handlePollout(socketConfig, clients, i, serverConfig) == -1)
					continue ;
			}
		}
	}
	// close(socketConfig.server_fd);
}