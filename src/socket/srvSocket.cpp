#include "socket.hpp"
#include "webserv.hpp"

sockaddr_in init_sockaddr_in(t_config serverConfig) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	(void)serverConfig;
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
void	parseBuffer(char *buffer, t_info_client &buffClient)
{
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
	std::cout << "buffClient.method = " << buffClient.method << std::endl;
	std::cout << "buffClient.url = " << buffClient.url << std::endl;
	std::cout << "buffClient.host = " << buffClient.host << std::endl;
}

void	handleDeconnexionClient(int i, struct pollfd *clients)
{
	close(clients[i].fd);
	clients[i].fd = 0;
}

void	checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients)
{
	for (int i = 0; i < MAX_CLIENTS; ++i)
	{
		if (clients[i].fd == 0) {
			clients[i].fd = accept(socketConfig.server_fd, (struct sockaddr *)&socketConfig.client_addr, &socketConfig.client_len);
			clients[i].events = POLLIN;
			break ;
		}
	}
}

void handleSocket(t_config serverConfig, t_socket &socketConfig)
{
	std::string	index = "./staticFiles/test.html";
	// std::map<int, struct pollfd> mapPoll;
	int client_count = 1;

	socketConfig.server_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (socketConfig.server_fd < 0)
		throw std::runtime_error("socket fail");
	socketConfig.server_addr = init_sockaddr_in(serverConfig);
	if (bind(socketConfig.server_fd, (const struct sockaddr *)&socketConfig.server_addr, sizeof(socketConfig.server_addr)) < 0)
		throw std::runtime_error("bind fail");
	if (listen(socketConfig.server_fd, 5) < 0)
		throw std::runtime_error("listen fail");
	struct pollfd clients[MAX_CLIENTS]; //Create struct
	memset(clients, 0, sizeof(clients));
	clients[0].fd = socketConfig.server_fd;
	clients[0].events = POLLIN;
	while(1) {
		if (poll(clients, client_count, -1) < 0)
			throw std::runtime_error("poll failed");
		for (int i = 0; i < client_count; i++) {
			if (clients[i].revents & POLLIN) {
				if (clients[i].fd == socketConfig.server_fd) {
					socketConfig.client_len = sizeof(socketConfig.client_addr);
					checkEmptyPlace(socketConfig, clients);
					if (client_count <= MAX_CLIENTS)
						client_count++;
				}
				else {
					char buffer[1024];
					if (recv(clients[i].fd, buffer, sizeof(buffer), 0) < 0)
						handleDeconnexionClient(i, clients);
					// std::cout << "buff = " << buffer << std::endl;
					t_info_client buffClient;
					parseBuffer(buffer, buffClient);
					std::string finalFile = readHtml(index);
					// std::cout << clients[i].fd << " " << socketConfig.server_fd << std::endl;
					if (send(clients[i].fd, finalFile.c_str(), finalFile.size(), 0) < 0)
						perror("send: ");
				}
			}
		}
		// close(clients[i].fd);
	}
	std::cout << "test8" << std::endl;
	// close(socketConfig.server_fd);
}

