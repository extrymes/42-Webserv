#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

extern sig_atomic_t stopRequested;

int handlePollout(t_socket &socketConfig, ClientRequest &clientRequest, int i) {
	std::string serverResponse = clientRequest.getServerResponse(i);
	long totalLen = serverResponse.size();
	long len = send(socketConfig.clients[i].fd, serverResponse.c_str(), totalLen, 0);
	if (len < 0) {
		handleClientDisconnection(i, socketConfig.clients);
		return -1;
	}
	clientRequest.clearServerResponse(i);
	if (len < totalLen) {
		clientRequest.setServerResponse(serverResponse.substr(len), i);
		socketConfig.clients[i].events = POLLOUT;
	} else
		handleClientDisconnection(i, socketConfig.clients);
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
		if (strncmp(location->path.c_str(), clientUrl.c_str(), pathSize) == 0) {
			clientRequest.setValueHeader("url", clientUrl.substr(pathSize - 1));
			return location;
		}
	}
	return location;
}

std::string	createUrl(std::vector<t_server>::iterator server, ClientRequest &clientRequest, std::string &clientUrl) {
	std::string file;
	std::vector<t_location>::iterator location = whichLocation(server, clientRequest, clientUrl);
	if (location == server->locations.end()) { //Si on ne trouve pas de partie location qui correspond à l'URL
		if (server->root.empty()) file = "";// s'il n'y a pas de root, je ne renvoie rien afin que l'erreur 404 soit affichée
		else { //sinon je prends le root et je test d'ajouter la partie index via la fonction addIndexOrUrl
			file = removeFirstSlash(server->root);
			addIndexOrUrl(server, server->indexes, clientRequest, file);
		}
	} else {
		file = location->root.empty() ? removeFirstSlash(server->root) + location->path : removeFirstSlash(location->root);
		addIndexOrUrl(server, location->indexes, clientRequest, file);
	}
	return file;
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
		if (recv(socketConfig.clients[i].fd, buffer, sizeof(buffer), 0) < 0)
			return (handleClientDisconnection(i, socketConfig.clients), -1);
		clientRequest.parseBuffer(buffer);
		std::vector<t_server>::iterator server = findIf(clientRequest.getValueHeader("port"), servers);
		if (server == servers.end())
			return -1;
		socketConfig.clients[i].events = POLLOUT;
		if (std::atol(clientRequest.getValueHeader("Content-Length").c_str()) > server->clientMaxBodySize) {
			clientRequest.setServerResponse(readHtml("413", server), i);
			return (clientRequest.clearBuff(), 0);
		}
		std::string clientUrl = clientRequest.getValueHeader("url"), output, file;
		if (isCGIFile(clientUrl) && clientRequest.getValueHeader("method") == "POST") {
			output = executeCGI(clientUrl, server->root, clientRequest.getBody());
			return (clientRequest.setServerResponse(httpResponse(output, "text/html", "200"), i), 0);
		}
		file = createUrl(server, clientRequest, clientUrl);
		std::cout << clientRequest.getValueHeader("method") << " " << file << " " << clientRequest.getValueHeader("protocol") << std::endl;
		if (clientRequest.getValueHeader("method") == "DELETE") {
			return (clientRequest.setServerResponse(httpResponse("", "", handleDeleteMethod(file)), i), 0);
		}
		clientRequest.setServerResponse(readHtml(file, server), i);
		clientRequest.clearHeader();
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
		std::cout << "[-] Running server " << CYAN << it->name << RESET << " on port " << CYAN << it->port << RESET << std::endl;
		++i;
	}
}

void handleSocket(std::vector<t_server> servers, t_socket &socketConfig) {
	ClientRequest clientRequest;
	socketConfig.clientCount = servers.size();
	memset(socketConfig.clients, 0, sizeof(socketConfig.clients));
	initSocket(socketConfig, servers);
	setupSignalHandler();
	while (!stopRequested) {
		if (poll(socketConfig.clients, socketConfig.clientCount, 1) < 0)
			continue;;
		for (int i = 0; i < socketConfig.clientCount; ++i) {
			if (socketConfig.clients[i].revents & POLLIN) {
				if (handlePollin(socketConfig, servers, clientRequest, i) == -1)
					continue;
			}
			if (socketConfig.clients[i].revents & POLLOUT) {
				if (handlePollout(socketConfig, clientRequest, i) == -1)
					continue;
			}
		}
	}
	std::cout << std::endl << RED << "[-] All servers are shut down" << RESET << std::endl;
	// close(socketConfig.server_fd);
}
