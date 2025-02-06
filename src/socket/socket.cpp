#include "ClientRequest.hpp"
#include "socket.hpp"
#include "cgiHandler.hpp"

extern sig_atomic_t stopRequested;

int handlePollout(t_socket &socketConfig, cMap &clientMap, int i) {
	std::string serverResponse = clientMap[i]->getServerResponse();
	long totalLen = serverResponse.size();
	long len = send(socketConfig.clients[i].fd, serverResponse.c_str(), totalLen, 0);
	if (len < 0) {
		handleClientDisconnection(i, socketConfig.clients, clientMap);
		return -1;
	}
	clientMap[i]->clearServerResponse();
	if (len < totalLen) {
		clientMap[i]->setServerResponse(serverResponse.substr(len));
		socketConfig.clients[i].events = POLLOUT;
	} else
		handleClientDisconnection(i, socketConfig.clients, clientMap);
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

locIt whichLocation(servIt it, ClientRequest *clientRequest, std::string clientUrl, std::string str) {
	locIt location = it->locations.begin();
	std::string goodUrl = createGoodUrl(clientUrl);
	for (; location != it->locations.end(); ++location) {
		std::string newLocation = urlWithoutSlash(location->path);
		std::string newClientUrl = urlWithoutSlash(goodUrl);
		const int pathSize = newLocation.size();
		if (strncmp(newLocation.c_str(), newClientUrl.c_str(), pathSize) == 0 && (newLocation.size() == newClientUrl.size())) {
			clientRequest->setValueHeader(str, goodUrl.substr(pathSize));
			return location;
		}
	}
	return location;
}

std::string	createUrl(servIt server, ClientRequest *clientRequest, std::string &clientUrl, locIt &location) {
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

std::string printAll(std::string str) {
	int len = str.size();
	for (int i = 0; i < len; i++) {
		std::cout << str[i];
	}
	return "";
}

int checkLenBody(ClientRequest *clientRequest, servIt server) {
	std::string test = clientRequest->getValueHeader("Content-Length");
	if (test.empty())
		return 1;
	long contentLenght = std::atol(test.c_str());
	if (contentLenght > server->clientMaxBodySize)
		return (clientRequest->setServerResponse(readHtml("413", server, CODE413)), 0);
	// std::cout << std::endl << "=============================================================" << std::endl << printAll(clientRequest->getBody()) << std::endl << "=============================================================" << std::endl;
	// std::cout << "clientRequest->getBody().size() = " << clientRequest->getBody().size() << std::endl;
	// std::cout << "contentLenght = " << contentLenght << std::endl;
	if ((size_t)contentLenght> clientRequest->getBody().size())
		return -1;
	return 1;
}

int handlePollin(t_socket &socketConfig, std::vector<t_server> &servers, cMap &clientMap, int i) {
	std::vector<int>::iterator it = std::find(socketConfig.serverFd.begin(), socketConfig.serverFd.end(), socketConfig.clients[i].fd);
	if (it != socketConfig.serverFd.end()) {
		socketConfig.clientLen = sizeof(socketConfig.clientAddr);
		checkEmptyPlace(socketConfig, clientMap, *it);
		if (socketConfig.clientCount < MAX_CLIENTS)
			socketConfig.clientCount++;
		return 0;
	}
	char buffer[4096] = {0};
	ssize_t size = recv(socketConfig.clients[i].fd, buffer, sizeof(buffer), 0);
	if (size <= 0)
		return (handleClientDisconnection(i, socketConfig.clients, clientMap), -1);
	clientMap[i]->parseBuffer(buffer, size);
	servIt server = findIf(clientMap[i]->getValueHeader("port"), servers);
	if (server == servers.end())
		return -1;
	socketConfig.clients[i].events = POLLOUT;
	int isToLarge = checkLenBody(clientMap[i], server);
	if (isToLarge < 1) {
		std::cout << "isToLarge = " << isToLarge << std::endl;
		return isToLarge;
	}
	std::string clientUrl = clientMap[i]->getValueHeader("url"), file, method;
	locIt location;
	file = createUrl(server, clientMap[i], clientUrl, location);
	if (location != server->locations.end() && !location->redirCode.empty())
		return (clientMap[i]->setServerResponse(redir(location)), 0);
	method = clientMap[i]->getValueHeader("method");

	std::cout << CYAN << method << RESET << " " << file << " " << clientMap[i]->getValueHeader("protocol") << std::endl;
	
	if (isCGIFile(clientUrl) && !isCGIAllowed(clientUrl, server, clientMap[i]))
		return (clientMap[i]->setServerResponse(readHtml("403", server, CODE403)), 0);
	if (method == "GET")
		handleGetMethod(server, location, clientMap[i], clientUrl, file);
	else if (method == "POST")
		handlePostMethod(server, location, clientMap[i], clientUrl, file);
	else if (method == "DELETE")
		handleDeleteMethod(server, clientMap[i], file);
	return 0;
}

void handleGetMethod(servIt server, locIt location, ClientRequest *clientRequest, std::string clientUrl, std::string file) {
	if (!isMethodAllowed("GET", server, clientRequest, clientUrl))
		return clientRequest->setServerResponse(readHtml("405", server, CODE405));
	if (!isCGIFile(clientUrl))
		return clientRequest->setServerResponse(readHtml(file, server, CODE200));
	std::string root = (location != server->locations.end() && !location->root.empty()) ? location->root : server->root;
	std::string output = executeCGI(clientUrl, root, clientRequest->getHeaderMap(), clientRequest->getBody());
	return clientRequest->setServerResponse(httpResponse(output, "text/html", CODE200));
}

void handlePostMethod(servIt server, locIt location, ClientRequest *clientRequest, std::string clientUrl, std::string file) {
	if (!isMethodAllowed("POST", server, clientRequest, ""))
		return clientRequest->setServerResponse(readHtml("405", server, CODE405));
	if (!isCGIFile(clientUrl))
		return clientRequest->setServerResponse(readHtml(file, server, CODE200));
	std::string root = (location != server->locations.end() && !location->root.empty()) ? location->root : server->root;
	std::string output = executeCGI(clientUrl, root, clientRequest->getHeaderMap(), clientRequest->getBody());
	return clientRequest->setServerResponse(httpResponse(output, "text/html", CODE200));
}

void handleDeleteMethod(servIt server, ClientRequest *clientRequest, std::string file) {
	if (!isMethodAllowed("DELETE", server, clientRequest, "")) //attention | potentiel timeout
		return clientRequest->setServerResponse(readHtml("405", server, CODE405));
	return clientRequest->setServerResponse(httpResponse("", "", handleDeleteMethod(file)));
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
	cMap clientMap;
	socketConfig.clientCount = servers.size();
	std::memset(socketConfig.clients, 0, sizeof(socketConfig.clients));
	initSocket(socketConfig, servers);
	setupSignalHandler();
	while (!stopRequested) {
		if (poll(socketConfig.clients, socketConfig.clientCount, 0) < 0)
			continue;
		for (int i = 0; i < socketConfig.clientCount; ++i) {
			if (socketConfig.clients[i].revents & POLLIN) {
				if (handlePollin(socketConfig, servers, clientMap, i) == -1) {
					socketConfig.clients[i].events = POLLIN;
					continue;
				}
				if (i >= (int)servers.size()) {
					clientMap[i]->clearHeader();
					clientMap[i]->clearBody();
				}
			}
			if (socketConfig.clients[i].revents & POLLOUT) {
				if (handlePollout(socketConfig, clientMap, i) == -1)
					continue;
			}
		}
	}
	std::cout << std::endl << RED << "[-] All servers are shut down" << RESET << std::endl;
	closeAllFds(socketConfig, clientMap);
}

void closeAllFds(t_socket &socketConfig, cMap &clientMap) {
	std::vector<int>::iterator it;
	for (it = socketConfig.serverFd.begin(); it != socketConfig.serverFd.end(); ++it)
		close(*it);
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (socketConfig.clients[i].fd != 0)
			delete clientMap[i];
		close(socketConfig.clients[i].fd);
	}
}
