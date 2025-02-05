#include "socket.hpp"

void initAddrInfo(std::vector<t_server> &servers, int i, struct addrinfo *hints, struct addrinfo **res) {
	std::memset(hints, 0, sizeof(struct addrinfo));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	if (int result = getaddrinfo(servers[i].host.c_str(), toString(servers[i].port).c_str(), hints, res) < 0)
			throw std::runtime_error("getaddrinfo fail" + (std::string)gai_strerror(result));
}

void handleClientDisconnection(int i, struct pollfd *clients, cMap &clientMap) {
	close(clients[i].fd);
	clients[i].fd = 0;
	clients[i].events = 0;
	clients[i].revents = 0;
	delete clientMap[i];
	// std::cout << "Closing a Client" << std::endl;
}

void checkEmptyPlace(t_socket &socketConfig, cMap &clientMap, int server_fd) {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (socketConfig.clients[i].fd == 0) {
			// std::cout << "Creating a new Client" << std::endl;
			socklen_t len = sizeof(socketConfig.clientAddr);
			clientMap[i] = new ClientRequest;
			socketConfig.clients[i].fd = accept(server_fd, (struct sockaddr *)&socketConfig.clientAddr, &len);
			socketConfig.clients[i].events = POLLIN;
			break;
		}
	}
}

void addIndexOrUrl(servIt server, std::vector<std::string> indexes, ClientRequest *clientRequest, std::string &path) {
	int err = 403;
	if (clientRequest->getValueHeader("url").size() <= 1) {
		std::vector<std::string>::iterator it = indexes.begin();
		if (path[path.size() - 1] != '/')
			path += '/';
		for (; it != indexes.end(); ++it) {
			err = 404;
			std::string temp = path + *it;
			std::ifstream file(temp.c_str());
			if (file.good()) {
				path = temp;
				return;
			}
		}
		isMap::iterator errNum = server->errorPages.find(err);
		path = errNum == server->errorPages.end() ? toString(err) : errNum->second;
	}
	else
		path += clientRequest->getValueHeader("url"); // Ex: root=www, url=etch-a-sketch/index.html
}

std::string toString(int nbr) {
	std::stringstream ss;
	ss << nbr;
	return ss.str();
}

std::string	handleDeleteMethod(std::string file) {
	int status = remove(file.c_str());
	if (status != 0) {
		perror("Error deleting file");
		return CODE404;
	}
	else {
		std::cout << GREEN << "success: the file has been deleted!" << RESET << std::endl;
		return CODE204;
	}
}

bool isMethodAllowed(std::string method, servIt server, ClientRequest *clientRequest) {
	std::string referer;
	if (method == "GET")
		referer = clientRequest->getValueHeader("url");
	else {
		referer = clientRequest->getValueHeader("Referer");
		if (clientRequest->getValueHeader("Origin").size() > 0)
			referer = referer.substr(clientRequest->getValueHeader("Origin").size() - 1);
	}
	locIt location = whichLocation(server, clientRequest, referer, "");
	if (location == server->locations.end() || location->allowedMethods.empty() || location->allowedMethods.find(method) != std::string::npos)
		return true;
	std::cout << RED << "error: method " << method << " is not allowed!" << RESET << std::endl;
	return false;
}

bool isCGIAllowed(std::string url, servIt server, ClientRequest *clientRequest) {
	std::string referer, extension;
	if (clientRequest->getValueHeader("method") == "GET")
		referer = clientRequest->getValueHeader("url");
	else {
		referer = clientRequest->getValueHeader("Referer");
		if (clientRequest->getValueHeader("Origin").size() > 0)
			referer = referer.substr(clientRequest->getValueHeader("Origin").size() - 1);
	}
	locIt location = whichLocation(server, clientRequest, referer, "");
	size_t idx = url.find_last_of('.');
	extension = url.substr(idx);
	if (location == server->locations.end() || location->cgiExtension.empty() || location->cgiExtension == extension)
		return true;
	std::cerr << RED << "error: CGI " << extension << " is not allowed!" << RESET << std::endl;
	return false;
}
