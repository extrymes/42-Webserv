#include "socket.hpp"

sockaddr_in init_sockaddr_in(std::vector<t_server> servers, int i) {
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(servers[i].port); // tmp
	return server_addr;
}

bool isError(std::string &index) {
	int nb = atoi(index.c_str());
	if (index.size() == 3 && (nb >= 300 && nb <= 527))
		return true;
	return false;
}

std::string httpResponse(std::string file, std::string ext) {
	std::string httpResponse = "HTTP/1.1 200 OK\r\n";
	httpResponse += "Content-Type: " + ext + "\r\n";
	httpResponse += "Content-Length: " + toString(file.length()) + "\r\n";
	httpResponse += "\r\n";
	httpResponse += file;
	return httpResponse;
}

std::string errorPage(std::string &err) {
	std::string file =
	"<!DOCTYPE html>\n"
		"<html lang=\"en\">\n"
		"<head>\n"
		"	<meta charset=\"UTF-8\">\n"
		"	<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n"
		"	<title>Error " + err + "</title>\n"
		"	<style>\n"
		"		body {\n"
		"			font-family: Arial, sans-serif;\n"
		"			text-align: center;\n"
		"			padding: 50px;\n"
		"			background-color: #f8f9fa;\n"
		"		}\n"
		"		h1 {\n"
		"			font-size: 3em;\n"
		"			color: #e74c3c;\n"
		"		}\n"
		"		p {\n"
		"			font-size: 1.5em;\n"
		"			color: #333;\n"
		"		}\n"
		"	</style>\n"
		"</head>\n"
		"<body>\n"
		"	<h1>Error " + err + "</h1>\n"
		"	<p>Oops! Something went wrong (HTTP Error " + err + ").</p>\n"
		"</body>\n"
		"</html>";
	return httpResponse(file, "text/html");
}

std::string readHtml(std::string &index, std::vector<t_server>::iterator server) {
	std::string	line;
	std::ifstream	infile(index.c_str());
	std::string	finalFile;
	std::ostringstream	oss;

	if (isError(index))
		return errorPage(index);
	if (!infile) {
		std::map<int, std::string>::iterator errNum = server->errorPages.find(404);
		std::string path = errNum == server->errorPages.end() ? toString(404) : errNum->second;
		return readHtml(path, server);
	}
	while (std::getline(infile, line))
		finalFile += line + "\n";
	return httpResponse(finalFile, checkExt(index));
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
	if (requestClient.getValue("url").size() <= 1) {
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
		path = errNum == server->errorPages.end() ? toString(err) : errNum->second;
	}
	else
		path += requestClient.getValue("url");
}

std::string toString(int nbr) {
	std::stringstream ss;
	ss << nbr;
	return ss.str();
}
