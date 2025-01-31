#include "socket.hpp"

void init_addrinfo(std::vector<t_server> servers, int i, struct addrinfo *hints, struct addrinfo **res) {
	memset(hints, 0, sizeof(struct addrinfo));
	hints->ai_family = AF_INET;
	hints->ai_socktype = SOCK_STREAM;
	if (int result = getaddrinfo(servers[i].host.c_str(), toString(servers[i].port).c_str(), hints, res) < 0)
			throw std::runtime_error("getaddrinfo fail" + (std::string)gai_strerror(result));
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
	httpResponse += "Connection: close\r\n";
	httpResponse += "\r\n";
	httpResponse += file;
	return httpResponse;
}

std::string errorPage(int error, std::vector<t_server>::iterator server) {
	std::string err;
	std::map<int, std::string>::iterator errNum = server->errorPages.find(error);
	if (errNum == server->errorPages.end())
		err = toString(error);
	else
		return (readHtml(errNum->second, server));
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

std::string readHtml(std::string index, std::vector<t_server>::iterator server) {
	std::string	line;
	std::string	finalFile;

	if (isError(index))
		return errorPage(std::atoi(index.c_str()), server);
	int isDir = open(index.c_str(), O_DIRECTORY);
	if (isDir > 0)
		return (close(isDir), errorPage(404, server));
	std::ifstream	infile(index.c_str());
	if (!infile)
		return errorPage(404, server);
	while (std::getline(infile, line))
		finalFile += line + "\n";
	infile.close();
	return httpResponse(finalFile, checkExt(index));
}

void handleClientDisconnection(int i, struct pollfd *clients) {
	close(clients[i].fd);
	clients[i].fd = 0;
	clients[i].events = 0;
	clients[i].revents = 0;
	std::cout << "Closing a Client" << std::endl;
}

void checkEmptyPlace(t_socket &socketConfig, struct pollfd *clients, int server_fd) {
	for (int i = 0; i < MAX_CLIENTS; ++i) {
		if (clients[i].fd == 0) {
			std::cout << "Creating a new Client" << std::endl;
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
		std::map<int, std::string>::iterator errNum = server->errorPages.find(err);
		path = errNum == server->errorPages.end() ? toString(err) : errNum->second;
	}
	else
		path += clientRequest.getValueHeader("url"); // Ex: root=www, url=etch-a-sketch/index.html
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

void	handleDeleteMethod(std::string file) {
	int status = remove(file.c_str());
	if (status != 0)
		perror("Error deleting file");
	else 
		std::cout << "File successfully deleted" << std::endl;
}