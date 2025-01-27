#include "socket.hpp"

sockaddr_in init_sockaddr_in(std::vector<t_server> servers, int i) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(servers[i].port); // tmp
	return server_addr;
}

std::string readHtml(std::string &index, std::vector<t_server> servers, std::string ext) {
	std::string	line;
	(void) ext;
	std::ifstream	infile(index.c_str());
	std::string	finalFile;
	std::ostringstream	oss;

	if (!infile) {
		std::map<int, std::string>::iterator it = servers[0].errorPages.find(404); //To Change
		std::string str = it->second;
		return readHtml(str, servers, checkExt(str));
	}
	while (std::getline(infile, line))
		finalFile += line + "\n";
	oss << finalFile.size();
	std::string contentLength = oss.str();
	std::string httpResponse =
	"HTTP/1.1 200 OK\r\n"
	// "Content-Type: " + ext + "\r\n"
	"Content-Length: " + contentLength + "\r\n"
	// "Connection: close\r\n"
	"\r\n" +
	finalFile;
	return httpResponse;
}

void handleClientDisconnection(int i, struct pollfd *clients) {
	close(clients[i].fd);
	clients[i].fd = 0;
	std::cout << "Closing a Client" << std::endl;
}

void checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients, int server_fd) {
	std::cout << "Creating a new Client" << std::endl;
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].fd == 0) {
			socklen_t len = sizeof(socketConfig.clientAddr);
			clients[i].fd = accept(server_fd, (struct sockaddr *)&socketConfig.clientAddr, &len);
			clients[i].events = POLLIN;
			break;
		}
	}
}

void addIndexOrUrl(std::vector<t_server>::iterator server, std::vector<std::string> indexes, RequestClient &requestClient, std::string &path, int flag) {
	int err = flag == 0 ? 403 : 404;
	if (requestClient.getUrl().size() <= 1) {
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
		std::map<int, std::string>::iterator errNum = server->errorPages.find(err);
		path = errNum == server->errorPages.end() ? "./web/" + toString(err) + ".html" : errNum->second;
	}
	else
		path += requestClient.getUrl();
}

std::string toString(int nbr) {
	std::stringstream ss;
	ss << nbr;
	return ss.str();
}