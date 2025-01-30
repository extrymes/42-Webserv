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
	(void) servers;
	std::string serverResponse = clientRequest.getServerResponse(i);
	long totalLen = serverResponse.size();
	long len = send(socketConfig.clients[i].fd, serverResponse.c_str(), totalLen, 0) ;
	if (len < 0) {
		handleClientDisconnection(i, socketConfig.clients);
		return -1;
	}
	clientRequest.clearServerResponse(i);
	if (len < totalLen) {
		clientRequest.setServerResponse(serverResponse.substr(len), i);
		socketConfig.clients[i].events = POLLOUT;
	} else
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
	(void)clientRequest;
	for (; location != it->locations.end(); ++location) {
		const int pathSize = location->path.size();
		if (strncmp(location->path.c_str(), clientUrl.c_str(), pathSize) == 0) {
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
		std::cout << buffer << std::endl;
		// std::cout << clientRequest.getValue("method") << " " << clientRequest.getValue("url") << " " << clientRequest.getValue("protocol") << std::endl;
		std::vector<t_server>::iterator server = findIf(clientRequest.getValue("port"), servers);
		if (server == servers.end())
			return -1;
		const std::string clientUrl = clientRequest.getValue("url");
		if (isCGIFile(clientUrl))
			executeCGI(clientUrl, server->root, clientRequest.getHeaders());
		std::string	file;
		std::vector<t_location>::iterator location = whichLocation(server, clientRequest, clientUrl);
		if (location == server->locations.end()) { //Si on ne trouve pas de partie location qui correspond à l'URL
			if (server->root.empty()) // s'il n'y a pas de root, je ne renvoie rien afin que l'erreur 404 soit affichée
				file = "";
			else { //sinon je prends le root et je test d'ajouter la partie index via la fonction addIndexOrUrl
				file = removeFirstSlash(server->root);
				addIndexOrUrl(server, server->indexes, clientRequest, file);
			}
		}
		else {
			file = location->root.empty() ? removeFirstSlash(server->root) + location->path : removeFirstSlash(location->root);
			addIndexOrUrl(server, location->indexes, clientRequest, file);
		}
		clientRequest.setServerResponse(readHtml(file, server), i);
		socketConfig.clients[i].events = POLLOUT;
	}
	return 0;
}

void initSocket(t_socket &socketConfig, std::vector<t_server> servers) {
	int	i = 0;
	struct addrinfo hints, *res;
	for (std::vector<t_server>::iterator it = servers.begin(); it != servers.end(); ++it) {
		init_addrinfo(servers, i, &hints, &res);

		socketConfig.serverFd.push_back(socket(res->ai_family, res->ai_socktype, 0));

		if (socketConfig.serverFd[i] < 0)
			throw std::runtime_error("socket fail");
		int opt = 1;
		if (setsockopt(socketConfig.serverFd[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt fail");
		if (bind(socketConfig.serverFd[i], res->ai_addr, res->ai_addrlen) < 0)
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
		if (sigintReceived(false)) {
			std::cout << "🚨 Server shutdown 🚨" << std::endl;
			return;
		}
		if (poll(socketConfig.clients, socketConfig.clientCount, 1) < 0)
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
