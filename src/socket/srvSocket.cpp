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

std::vector<t_server>::iterator findIf(std::string port, std::vector<t_server> &servers)
{
	std::vector<t_server>::iterator it = servers.begin();
	for (; it != servers.end(); ++it) {
		if (it->port == atoi(port.c_str()))
			return it;
	}
	return it;
}

std::vector<t_location>::iterator	whichLocation(std::vector<t_server>::iterator it, RequestClient &requestClient) {
	std::vector<t_location>::iterator location = it->locations.begin();
	for (; location != it->locations.end(); ++location) {
		if (location->path == requestClient.getUrl())
			return location;
	}
	return location;
}

int	handlePollin(t_socket &socketConfig, struct pollfd *clients, int i, int &client_count, std::vector<t_server> servers, RequestClient &requestClient) {
	std::vector<int>::iterator it = std::find(socketConfig.server_fd.begin(), socketConfig.server_fd.end(), clients[i].fd);
	if (it != socketConfig.server_fd.end()) {
		socketConfig.client_len = sizeof(socketConfig.client_addr);
		checkEmptyPlace(socketConfig, clients, *it);
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
		std::vector<t_server>::iterator it = findIf(requestClient.getPort(), servers);
		if (it == servers.end())
			return -1;
		std::vector<t_location>::iterator location = whichLocation(it, requestClient);
		std::string	file;
		std::string root = "web/etch-a-sketch";
		if (location == it->locations.end()) {
			if (requestClient.getUrl().size() > 1) {
				file = root + requestClient.getUrl(); // serveur.root
			}
			else
				file = "./web/index.html";
		}
		else {
			std::cerr << "path " << location->path << std::endl;
			root = location->root.empty() ? location->path : location->root;
			std::string index = location->indexes.size() == 0 ? "/index.html" : location->indexes[0];
			if (requestClient.getUrl() == "/" || location->path == requestClient.getUrl())
				file = root + index;
			else
				file = root + requestClient.getUrl();
		}
		std::cout << file << std::endl;
		requestClient.setResponseServer(readHtml(file, servers, checkExt(file)));
		clients[i].events = POLLIN | POLLOUT;
	}
	return 0;
}

void	initSocket(t_socket &socketConfig, std::vector<t_server> servers, struct pollfd *clients) {
	int	i = 0;
	for (std::vector<t_server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		socketConfig.server_fd.push_back(socket(AF_INET, SOCK_STREAM, 0));
		if (socketConfig.server_fd[i] < 0)
			throw std::runtime_error("socket fail");
		int opt = 1;
		if (setsockopt(socketConfig.server_fd[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt fail");
		socketConfig.server_addr.push_back(init_sockaddr_in(servers, i));
		if (bind(socketConfig.server_fd[i], (const struct sockaddr *)&socketConfig.server_addr[i], sizeof(socketConfig.server_addr[i])) < 0)
			throw std::runtime_error("bind fail");
		if (listen(socketConfig.server_fd[i], 5) < 0)
			throw std::runtime_error("listen fail");
		clients[i].fd = socketConfig.server_fd[i];
		clients[i].events = POLLIN | POLLOUT;
		++i;
	}
}

void handleSocket(std::vector<t_server> servers, t_socket &socketConfig) {
	RequestClient requestClient;
	int	client_count = servers.size();
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
