#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

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

int handlePollout(t_socket &socketConfig, std::vector<t_server> servers, ClientRequest &clientRequest, int i) {
	// std::cout << "i Pollout = " << i << std::endl;
	(void)servers;
	std::string serverResponse = clientRequest.getServerResponse(i);
	if (send(socketConfig.clients[i].fd, serverResponse.c_str(), serverResponse.size(), 0) < 0) {
		handleClientDisconnection(i, socketConfig.clients);
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

std::vector<t_location>::iterator whichLocation(std::vector<t_server>::iterator it, ClientRequest &clientRequest, std::string clientUrl) {
	std::vector<t_location>::iterator location = it->locations.begin();
	for (; location != it->locations.end(); ++location) {
		const int pathSize = location->path.size();
		if (strncmp(location->path.c_str(), clientUrl.c_str(), pathSize) == 0 && (clientUrl[pathSize - 1] == '/' || clientUrl[pathSize - 1] == '\0')) {
			clientRequest.setValue("url", clientUrl.substr(pathSize - 1));
			return location;
		}
	}
	return location;
}

int handlePollin(t_socket &socketConfig, std::vector<t_server> servers, ClientRequest &clientRequest, int i) {
	std::vector<int>::iterator it = std::find(socketConfig.serverFd.begin(), socketConfig.serverFd.end(), socketConfig.clients[i].fd);
	if (it != socketConfig.serverFd.end()) {
		socketConfig.clientLen = sizeof(socketConfig.clientAddr);
		checkEmptyPlace(socketConfig, socketConfig.clients, *it);
		if (socketConfig.clientCount <= MAX_CLIENTS)
			socketConfig.clientCount++;
	} else {
		char buffer[4096];
		if (recv(socketConfig.clients[i].fd, buffer, sizeof(buffer), 0) < 0) {
			handleClientDisconnection(i, socketConfig.clients);
			return -1;
		}
		clientRequest.parseBuffer(buffer);
		// std::cout << "buffer = " << buffer << std::endl;
		std::cout << clientRequest.getValue("method") << " " << clientRequest.getValue("url") << " " << clientRequest.getValue("protocol") << std::endl;
		std::vector<t_server>::iterator server = findIf(clientRequest.getValue("port"), servers);
		if (server == servers.end())
			return -1;
		const std::string clientUrl = clientRequest.getValue("url");
		std::vector<t_location>::iterator location = whichLocation(server, clientRequest, clientUrl);
		if (isCGIFile(clientUrl))
			executeCGI(clientUrl, clientRequest.getHeaders());
		std::string	file;
		if (location == server->locations.end()) {
			file = server->root.empty() ? server->errorPages.find(404)->second : server->root; //err second seg fault
			addIndexOrUrl(server, server->indexes, clientRequest, file, 0);
		} else {
			file = location->root.empty() ? location->path : location->root;
			addIndexOrUrl(server, location->indexes, clientRequest, file, 1);
		}
		clientRequest.setServerResponse(readHtml(file, server), i);
		socketConfig.clients[i].events = POLLOUT;
	}
	return 0;
}

void initSocket(t_socket &socketConfig, std::vector<t_server> servers) {
	int	i = 0;
	for (std::vector<t_server>::iterator it = servers.begin(); it != servers.end(); ++it) {
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
	ClientRequest clientRequest;
	socketConfig.clientCount = servers.size();
	memset(socketConfig.clients, 0, sizeof(socketConfig.clients));
	initSocket(socketConfig, servers);
	while (1) {
		if (poll(socketConfig.clients, socketConfig.clientCount, -1) < 0)
			throw std::runtime_error("poll failed");
		for (int i = 0; i < socketConfig.clientCount; ++i) {
			if (socketConfig.clients[i].revents & POLLIN) {
				if (handlePollin(socketConfig, servers, clientRequest, i) == -1)
					continue;
			}
			if (socketConfig.clients[i].revents & POLLOUT) {
				if (handlePollout(socketConfig, servers, clientRequest, i) == -1)
 					continue;
			}
		}
	}
	// close(socketConfig.server_fd);
}
