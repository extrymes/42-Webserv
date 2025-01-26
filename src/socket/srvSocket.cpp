#include "socket.hpp"
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

int handlePollout(t_socket &socketConfig, std::vector<t_server> servers, RequestClient &requestClient, int i) {
	(void)servers;
	std::cout << "port = " << requestClient.getPort() << std::endl;
	if (send(socketConfig.clients[i].fd, requestClient.getResponseServer().c_str(), requestClient.getResponseServer().size(), 0) < 0) {
		handleDeconnexionClient(i, socketConfig.clients);
		return -1;
	}
	socketConfig.clients[i].events = POLLIN;
	return 0;
}

std::vector<t_server>::iterator findIf(std::string port, std::vector<t_server> &servers) {
	std::vector<t_server>::iterator it = servers.begin();
	for (; it != servers.end(); ++it) {
		if (it->port == atoi(port.c_str()))
			return it;
	}
	return it;
}

std::vector<t_location>::iterator whichLocation(std::vector<t_server>::iterator it, RequestClient &requestClient) {
	std::vector<t_location>::iterator location = it->locations.begin();
	for (; location != it->locations.end(); ++location) {
		if (location->path == requestClient.getUrl())
			return location;
	}
	return location;
}

int handlePollin(t_socket &socketConfig, std::vector<t_server> servers, RequestClient &requestClient, int i) {
	std::vector<int>::iterator it = std::find(socketConfig.serverFd.begin(), socketConfig.serverFd.end(), socketConfig.clients[i].fd);
	if (it != socketConfig.serverFd.end()) {
		socketConfig.clientLen = sizeof(socketConfig.clientAddr);
		checkEmptyPlace(socketConfig, socketConfig.clients, *it);
		if (socketConfig.clientCount <= MAX_CLIENTS)
			socketConfig.clientCount++;
	}
	else {
		char buffer[4096];
		if (recv(socketConfig.clients[i].fd, buffer, sizeof(buffer), 0) < 0) {
			handleDeconnexionClient(i, socketConfig.clients);
			return -1;
		}
		requestClient.parseBuffer(buffer);
		std::vector<t_server>::iterator it = findIf(requestClient.getPort(), servers);
		if (it == servers.end())
			return -1;
		std::vector<t_location>::iterator location = whichLocation(it, requestClient);
		std::string	file;
		std::string root = "web/etch-a-sketch";
		if (location == it->locations.end())
			file = requestClient.getUrl().size() > 1 ? root + requestClient.getUrl() : "./web/index.html";
		else {
			std::cerr << "path " << location->path << std::endl;
			root = location->root.empty() ? location->path : location->root;
			std::string index = location->indexes.size() == 0 ? "/index.html" : location->indexes[0];
			location->path == requestClient.getUrl() ? file = root + index : file = root + requestClient.getUrl();
		}
		std::cout << file << std::endl;
		requestClient.setResponseServer(readHtml(file, servers, checkExt(file)));
		socketConfig.clients[i].events = POLLIN | POLLOUT;
	}
	return 0;
}

void initSocket(t_socket &socketConfig, std::vector<t_server> servers) {
	int	i = 0;
	for (std::vector<t_server>::iterator it = servers.begin(); it != servers.end(); ++it)
	{
		socketConfig.serverFd.push_back(socket(AF_INET, SOCK_STREAM, 0));
		if (socketConfig.serverFd[i] < 0)
			throw std::runtime_error("socket fail");
		int opt = 1;
		if (setsockopt(socketConfig.serverFd[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt fail");
		socketConfig.serverAddr.push_back(init_sockaddr_in(servers, i));
		if (bind(socketConfig.serverFd[i], (const struct sockaddr *)&socketConfig.serverAddr[i], sizeof(socketConfig.serverAddr[i])) < 0)
			throw std::runtime_error("bind fail");
		if (listen(socketConfig.serverFd[i], 5) < 0)
			throw std::runtime_error("listen fail");
		socketConfig.clients[i].fd = socketConfig.serverFd[i];
		socketConfig.clients[i].events = POLLIN | POLLOUT;
		++i;
	}
}

void handleSocket(std::vector<t_server> servers, t_socket &socketConfig) {
	RequestClient requestClient;
	socketConfig.clientCount = servers.size();
	memset(socketConfig.clients, 0, sizeof(socketConfig.clients));
	initSocket(socketConfig, servers);
	while (1) {
		if (poll(socketConfig.clients, socketConfig.clientCount, -1) < 0)
			throw std::runtime_error("poll failed");
		for (int i = 0; i < socketConfig.clientCount; i++) {
			if (socketConfig.clients[i].revents & POLLIN) {
				if (handlePollin(socketConfig, servers, requestClient, i) == -1)
					continue;
			}
			if (socketConfig.clients[i].revents & POLLOUT) {
				if (handlePollout(socketConfig, servers, requestClient, i) == -1)
					continue;
			}
		}
	}
	// close(socketConfig.server_fd);
}
