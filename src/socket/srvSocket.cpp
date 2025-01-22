#include "socket.hpp"
#include "webserv.hpp"

sockaddr_in init_sockaddr_in(std::vector<t_server> servers) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	(void)servers;
	server_addr.sin_port = htons(8082); // tmp
	return (server_addr);
}

std::string readHtml(std::string index)
{
	std::string	line;
	std::ifstream	infile(index.c_str());
	std::string	finalFile;
	std::ostringstream oss;

	if (!infile)
		throw(std::invalid_argument("Failed to open the infile."));
	while (std::getline(infile, line))
		finalFile += line + "\n";
	oss << finalFile.size();
	std::string contentLength = oss.str();
	std::string httpResponse =
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: " + contentLength + "\r\n"
	"Connection: close\r\n"
	"\r\n" +
	finalFile;
	return httpResponse;
}

void handleSocket(std::vector<t_server> servers, t_socket &socketConfig)
{
	std::string	index = "./staticFiles/test.html";

	socketConfig.server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.server_fd < 0)
		throw std::runtime_error("socket fail");
	socketConfig.server_addr = init_sockaddr_in(servers);
	if (bind(socketConfig.server_fd, (const struct sockaddr *)&socketConfig.server_addr, sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("bind fail");
	if (listen(socketConfig.server_fd, 5) < 0)
		throw std::runtime_error("listen fail");
	while (1) {
		socketConfig.client_len = sizeof(socketConfig.client_addr);
		socketConfig.client_fd = accept(socketConfig.server_fd, (struct sockaddr *)&socketConfig.client_addr, &socketConfig.client_len);
		char buffer[1024];
		if (recv(socketConfig.client_fd, buffer, sizeof(buffer), 0) < 0)
			std::cerr << "bonjour" << std::endl;
		// std::cout << "buff = " << buffer << std::endl;
		std::string finalFile = readHtml(index);
		if (send(socketConfig.client_fd, finalFile.c_str(), finalFile.size(), 0) < 0)
			throw std::runtime_error("send failed");
		close(socketConfig.client_fd);
	}
	close(socketConfig.server_fd);
}
