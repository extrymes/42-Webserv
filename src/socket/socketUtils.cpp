#include "socket.hpp"

void initAddrInfo(std::vector<t_server> servers, int i, struct addrinfo *hints, struct addrinfo **res) {
	memset(hints, 0, sizeof(struct addrinfo));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	if (int result = getaddrinfo(servers[i].host.c_str(), toString(servers[i].port).c_str(), hints, res) < 0)
			throw std::runtime_error("getaddrinfo fail" + (std::string)gai_strerror(result));
}

void handleClientDisconnection(int i, struct pollfd *clients) {
	close(clients[i].fd);
	clients[i].fd = 0;
	clients[i].events = 0;
	clients[i].revents = 0;
	// std::cout << "Closing a Client" << std::endl;
}

void checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients, int server_fd) {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].fd == 0) {
			// std::cout << "Creating a new Client" << std::endl;
			socklen_t len = sizeof(socketConfig.clientAddr);
			clients[i].fd = accept(server_fd, (struct sockaddr *)&socketConfig.clientAddr, &len);
			clients[i].events = POLLIN;
			break;
		}
	}
}

void addIndexOrUrl(std::vector<t_server>::iterator server, std::vector<std::string> indexes, ClientRequest &clientRequest, std::string &path) {
	int err = 403;
	if (clientRequest.getValueHeader("url").size() <= 1) {
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
		path += removeFirstSlash(clientRequest.getValueHeader("url")); // Ex: root=www, url=etch-a-sketch/index.html
}

std::string toString(int nbr) {
	std::stringstream ss;
	ss << nbr;
	return ss.str();
}

std::string	removeFirstSlash (std::string str) {
	if (str[0] == '/')
		return str.substr(1);
	return str;
}

std::string	handleDeleteMethod(std::string file) {
	int status = remove(file.c_str());
	if (status != 0) {
		perror("Error deleting file");
		return CODE404;
	}
	else {
		std::cout << GREEN << "File successfully deleted" << RESET << std::endl;
		return CODE204;
	}
}

bool isMethodAllowed(std::string method, std::vector<t_server>::iterator server, ClientRequest &clientRequest) {
	std::string referer;
	if (method != "GET") {
		referer = clientRequest.getValueHeader("Referer");
		// std::cout << clientRequest.getValueHeader("Origin") << std::endl;
		referer = referer.substr(clientRequest.getValueHeader("Origin").size() - 1);
	} else 
		referer = clientRequest.getValueHeader("url");
	// std::cout << "referer = " << referer << std::endl;
	std::vector<t_location>::iterator location = whichLocation(server, clientRequest, referer, "");
	if (location == server->locations.end() || location->allowedMethods.empty() || location->allowedMethods.find(method) != std::string::npos)
		return true;
	std::cout << RED <<  "Method " << method << " is not allowed !" << RESET << std::endl;
	return false;
}
