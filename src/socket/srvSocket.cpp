#include "socket.hpp"
#include "webserv.hpp"
#include "requestClient.hpp"

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

int handlePollout(t_socket &socketConfig, struct pollfd *clients, int i, std::vector<t_server> servers, RequestClient &requestClient) {
	(void)servers;
	(void)socketConfig;
	std::cout << "port = " << requestClient.getPort() << std::endl;
	if (send(clients[i].fd, requestClient.getResponseServer().c_str(), requestClient.getResponseServer().size(), 0) < 0) {
		handleDeconnexionClient(i, clients);
		return -1;
	}
	clients[i].events = POLLIN;
	return 0;
}

int	handlePollin(t_socket &socketConfig, struct pollfd *clients, int i, int &client_count, std::vector<t_server> servers, RequestClient &requestClient) {

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
		requestClient.parseBuffer(buffer);
		std::string root = servers[0].locations[0].root.empty() ? servers[0].locations[0].path : servers[0].locations[0].root;
		std::string index = servers[0].locations[0].indexes.size() == 0 ? "/index.html" : servers[0].locations[0].indexes[0];
		std::string	file;
		if (requestClient.getUrl() == "/")
			file = root + index;
		else
			file = root + requestClient.getUrl();
		requestClient.setResponseServer(readHtml(file, servers, checkExt(file)));
		clients[i].events = POLLIN | POLLOUT;
	}
	return 0;
}

void	initSocket(t_socket &socketConfig, std::vector<t_server> servers, struct pollfd *clients) {
	socketConfig.server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.server_fd < 0)
		throw std::runtime_error("socket fail");
	int opt = 1;
	if (setsockopt(socketConfig.server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throw std::runtime_error("setsockopt fail");
	socketConfig.server_addr = init_sockaddr_in(servers);
	if (bind(socketConfig.server_fd, (const struct sockaddr *)&socketConfig.server_addr, sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("bind fail");
	if (listen(socketConfig.server_fd, 5) < 0)
		throw std::runtime_error("listen fail");
	clients[0].fd = socketConfig.server_fd;
	clients[0].events = POLLIN | POLLOUT;
}

void handleSocket(std::vector<t_server> servers, t_socket &socketConfig) {
	RequestClient requestClient;
	int	client_count = 1;
	struct pollfd	clients[MAX_CLIENTS]; //Create struct
	memset(clients, 0, sizeof(clients));
	initSocket(socketConfig, servers, clients);
	while (1) {
		if (poll(clients, client_count, -1) < 0)
			throw std::runtime_error("poll failed");
		for (int i = 0; i < client_count; i++) {
			if (clients[i].revents & POLLIN) {
				if (handlePollin(socketConfig, clients, i, client_count, servers, requestClient) == -1)
					continue ;
			}
			if (clients[i].revents & POLLOUT) {
				if (handlePollout(socketConfig, clients, i, servers, requestClient) == -1)
					continue ;
			}
		}
	}
	// close(socketConfig.server_fd);
}
