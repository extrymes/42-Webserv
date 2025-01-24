#include "webserv.hpp"

sockaddr_in init_sockaddr_in(t_config serverConfig) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	(void)serverConfig;
	server_addr.sin_port = htons(8080); // tmp
	return (server_addr);
}

std::string readHtml(std::string &index, t_config &serverConfig, std::string ext) {
	(void)serverConfig;
	std::string	line;
	std::ifstream	infile(index.c_str());
	std::string	finalFile;
	std::ostringstream	oss;

	if (!infile) {
		std::string str = "./web/404.html";
		return readHtml(str, serverConfig, ".html"); //To modify
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

void	handleDeconnexionClient(int i, struct pollfd *clients) {
	close(clients[i].fd);
	clients[i].fd = 0;
}

void	checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients) {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].fd == 0) {
			clients[i].fd = accept(socketConfig.server_fd, (struct sockaddr *)&socketConfig.client_addr, &socketConfig.client_len);
			clients[i].events = POLLIN;
			break ;
		}
	}
}

void	parseBuffer(char *buffer, t_info_client &buffClient) {
	std::string	firstLine, secondLine;
	std::stringstream infileBuff(buffer);
	if (!infileBuff)
		throw std::runtime_error("opening buffer failed");
	std::getline(infileBuff, firstLine);
	std::getline(infileBuff, secondLine);
	std::stringstream first(firstLine);
	if (!first)
		throw std::runtime_error("opening buffer failed");
	std::getline(first, buffClient.method, ' ');
	std::getline(first, buffClient.url, ' ');
	std::stringstream second(secondLine);
	if (!second)
		throw std::runtime_error("opening buffer failed");
	std::string tmp;
	std::getline(second, tmp, ' ');
	std::getline(second, buffClient.host);
}