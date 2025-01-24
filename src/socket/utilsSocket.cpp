#include "webserv.hpp"

sockaddr_in init_sockaddr_in(std::vector<t_server> servers, int i) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(servers[i].port); // tmp
	return server_addr;
}

std::string readHtml(std::string &index, std::vector<t_server> servers, std::string ext) {
	std::string	line;
	std::ifstream	infile(index.c_str());
	std::string	finalFile;
	std::ostringstream	oss;

	// std::cerr << "index = " << index << std::endl;
	if (!infile) {
		std::map<int, std::string>::iterator it = servers[0].errorPages.find(404);
		std::string str = it->second;
		return readHtml(str, servers, checkExt(str));
	}
	while (std::getline(infile, line))
		finalFile += line + "\n";
	oss << finalFile.size();
	std::string contentLength = oss.str();
	std::string httpResponse =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: " + ext + "\r\n"
	"Content-Length: " + contentLength + "\r\n"
	// "Connection: close\r\n"
	"\r\n" +
	finalFile;
	return httpResponse;
}

void handleDeconnexionClient(int i, struct pollfd *clients) {
	close(clients[i].fd);
	clients[i].fd = 0;
}

void checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients, int server_fd) {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].fd == 0) {
			socklen_t len = sizeof(socketConfig.client_addr);
			clients[i].fd = accept(server_fd, (struct sockaddr *)&socketConfig.client_addr, &len);
			clients[i].events = POLLIN;
			break;
		}
	}
}
