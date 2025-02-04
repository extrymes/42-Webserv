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

servIt findIf(std::string port, std::vector<t_server> &servers) {
	servIt it = servers.begin();
	for (; it != servers.end(); ++it) {
		if (it->port == atoi(port.c_str()))
			return it;
	}
	return it;
}

std::string createGoodUrl(std::string oldUrl) {
	std::string goodUrl;
	size_t first = oldUrl.find_first_not_of('/');
	for (size_t i = first; i < oldUrl.size(); i++) {
		if ((oldUrl[i] != '/' && std::isprint(oldUrl[i])) || (oldUrl[i] == '/' && isalnum(oldUrl[i + 1])))
			goodUrl += oldUrl[i];
	}
	// std::cout << "good url = " << goodUrl << std::endl;
	return goodUrl;
}

std::string urlWithoutSlash(std::string location) {
	std::string newLocation;
	for(size_t i = location.find_first_not_of('/'); i < location.size(); i++) {
		if (location[i] == '/')
			break ;
		newLocation += location[i];
	}
	return newLocation;
}

locIt whichLocation(servIt it, ClientRequest &clientRequest, std::string clientUrl, std::string str) {
	locIt location = it->locations.begin();
	std::string goodUrl = createGoodUrl(clientUrl);
	for (; location != it->locations.end(); ++location) {
		std::string newLocation = urlWithoutSlash(location->path);
		std::string newClientUrl = urlWithoutSlash(goodUrl);
		const int pathSize = newLocation.size();
		if (strncmp(newLocation.c_str(), newClientUrl.c_str(), pathSize) == 0 && (newLocation.size() == newClientUrl.size())) {
			clientRequest.setValueHeader(str, goodUrl.substr(pathSize));
			return location;
		}
	}
	return location;
}

std::string	createUrl(servIt server, ClientRequest &clientRequest, std::string &clientUrl, locIt &location) {
	std::string file;
	location = whichLocation(server, clientRequest, clientUrl, "url");
	if (location == server->locations.end()) { //Si on ne trouve pas de partie location qui correspond à l'URL
		if (server->root.empty()) file = "";// s'il n'y a pas de root, je ne renvoie rien afin que l'erreur 404 soit affichée
		else { //sinon je prends le root et je test d'ajouter la partie index via la fonction addIndexOrUrl
			file = server->root;
			addIndexOrUrl(server, server->indexes, clientRequest, file);
		}
	} else {
		file = location->root.empty() ? server->root + location->path : location->root;
		addIndexOrUrl(server, location->indexes, clientRequest, file);
	}
	return file;
}

int handlePollin(t_socket &socketConfig, std::vector<t_server> &servers, ClientRequest &clientRequest, int i) {
	std::vector<int>::iterator it = std::find(socketConfig.serverFd.begin(), socketConfig.serverFd.end(), socketConfig.clients[i].fd);
	if (it != socketConfig.serverFd.end()) {
		socketConfig.clientLen = sizeof(socketConfig.clientAddr);
		checkEmptyPlace(socketConfig, socketConfig.clients, *it);
		if (socketConfig.clientCount < MAX_CLIENTS)
			socketConfig.clientCount++;
		return 0;
	}
	char buffer[4096];
	if (recv(socketConfig.clients[i].fd, buffer, sizeof(buffer), 0) < 0)
		return (handleClientDisconnection(i, socketConfig.clients), -1);
	// std::cout << buffer << std::endl;
	clientRequest.parseBuffer(buffer);
	servIt server = findIf(clientRequest.getValueHeader("port"), servers);
	if (server == servers.end())
		return -1;
	socketConfig.clients[i].events = POLLOUT;
	if (std::atol(clientRequest.getValueHeader("Content-Length").c_str()) > server->clientMaxBodySize) {
		clientRequest.setServerResponse(readHtml("413", server, CODE413), i);
		return (clientRequest.clearBuff(), 0);
	}
	std::string clientUrl = clientRequest.getValueHeader("url"), file, method;
	locIt location;
	file = createUrl(server, clientRequest, clientUrl, location);
	if (location != server->locations.end() && !location->redirCode.empty()) {
		return (clientRequest.setServerResponse(redir(location), i), 0);
	}
	method = clientRequest.getValueHeader("method");
	std::cout << CYAN << method << RESET << " " << file << " " << clientRequest.getValueHeader("protocol") << std::endl;
	if (isCGIFile(clientUrl) && !isCGIAllowed(clientUrl, server, clientRequest))
		return (clientRequest.setServerResponse(readHtml("403", server, CODE403), i), 0);
	if (method == "GET")
		handleGetMethod(server, location, clientRequest, clientUrl, file, i);
	else if (method == "POST")
		handlePostMethod(server, location, clientRequest, clientUrl, file, i);
	else if (method == "DELETE")
		handleDeleteMethod(server, clientRequest, file, i);
	clientRequest.clearHeader();
	return 0;
}

void handleGetMethod(servIt server, locIt location, ClientRequest &clientRequest, std::string clientUrl, std::string file, int i) {
	if (!isMethodAllowed("GET", server, clientRequest))
		return clientRequest.setServerResponse(readHtml("405", server, CODE405), i);
	if (!isCGIFile(clientUrl))
		return clientRequest.setServerResponse(readHtml(file, server, CODE200), i);
	std::string root = (location != server->locations.end() && !location->root.empty()) ? location->root : server->root;
	std::string output = executeCGI(clientUrl, root, clientRequest.getHeaderMap(), clientRequest.getBody()); //server root seg
	return clientRequest.setServerResponse(httpResponse(output, "text/html", CODE200), i);
}

void handlePostMethod(servIt server, locIt location, ClientRequest &clientRequest, std::string clientUrl, std::string file, int i) {
	if (!isMethodAllowed("POST", server, clientRequest))
		return clientRequest.setServerResponse(readHtml("405", server, CODE405), i);
	if (!isCGIFile(clientUrl))
		return clientRequest.setServerResponse(readHtml(file, server, CODE200), i);
	std::string root = (location != server->locations.end() && !location->root.empty()) ? location->root : server->root;
	std::string output = executeCGI(clientUrl, root, clientRequest.getHeaderMap(), clientRequest.getBody());
	return clientRequest.setServerResponse(httpResponse(output, "text/html", CODE200), i);
}

void handleDeleteMethod(servIt server, ClientRequest &clientRequest, std::string file, int i) {
	if (!isMethodAllowed("DELETE", server, clientRequest))
		return clientRequest.setServerResponse(readHtml("405", server, CODE405), i);
	return clientRequest.setServerResponse(httpResponse("", "", handleDeleteMethod(file)), i);
}

void initSocket(t_socket &socketConfig, std::vector<t_server> &servers) {
	int	i = 0;
	struct addrinfo hints, *res;
	for (servIt it = servers.begin(); it != servers.end(); ++it) {
		initAddrInfo(servers, i, &hints, &res);

		socketConfig.serverFd.push_back(socket(res->ai_family, res->ai_socktype, 0));

		if (socketConfig.serverFd[i] < 0)
			throw std::runtime_error("socket fail");
		int opt = 1;
		if (setsockopt(socketConfig.serverFd[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
			throw std::runtime_error("setsockopt fail");
		if (bind(socketConfig.serverFd[i], res->ai_addr, res->ai_addrlen) < 0)
			throw std::runtime_error("bind fail");
		freeaddrinfo(res);
		if (listen(socketConfig.serverFd[i], 5) < 0)
			throw std::runtime_error("listen fail");
		socketConfig.clients[i].fd = socketConfig.serverFd[i];
		socketConfig.clients[i].events = POLLIN | POLLOUT;
		std::cout << "[-] Running server " << CYAN << it->name << RESET << " on port " << CYAN << it->port << RESET << std::endl;
		++i;
	}
}

void handleSocket(std::vector<t_server> &servers, t_socket &socketConfig) {
	ClientRequest clientRequest;
	socketConfig.clientCount = servers.size();
	std::memset(socketConfig.clients, 0, sizeof(socketConfig.clients));
	initSocket(socketConfig, servers);
	setupSignalHandler();
	while (!stopRequested) {
		if (poll(socketConfig.clients, socketConfig.clientCount, 0) < 0)
			continue;
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
	closeAllFds(socketConfig);
	// close(socketConfig.server_fd);
}

void closeAllFds(t_socket &socketConfig) {
	std::vector<int>::iterator it;
	for (it = socketConfig.serverFd.begin(); it != socketConfig.serverFd.end(); ++it)
		close(*it);
	for (int i = 0; i < MAX_CLIENTS; ++i)
		close(socketConfig.clients[i].fd);
}
