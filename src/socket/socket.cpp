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
		if (location[i] == '/' && !isalnum(location[i + 1]))
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
		if (std::strncmp(newLocation.c_str(), newClientUrl.c_str(), pathSize) == 0 && newClientUrl[pathSize] != '.') {
			clientRequest->setValueHeader(str, goodUrl.substr(pathSize));
			return location;
		}
	}
	return location;
}

std::string	createUrl(servIt server, ClientRequest *clientRequest, locIt &location) {
	std::string file;
	if (location == server->locations.end()) {
		if (server->root.empty()) file = "";
		else {
			file = server->root;
			addIndexOrUrl(server, server->indexes, clientRequest, file, location);
		}
	} else {
		file = location->root.empty() ? server->root + location->path : location->root;
		addIndexOrUrl(server, location->indexes, clientRequest, file, location);
	}
	return file;
}

int checkLenBody(ClientRequest *clientRequest, servIt server, ssize_t size) {
	std::string requestLength = clientRequest->getValueHeader("Content-Length");
	long contentLength = std::atol(requestLength.c_str());
	if (contentLength > server->clientMaxBodySize || clientRequest->getValueHeader("url").size() == 4096) {
		if (clock() - clientRequest->getStart() < 1000 && size == 4096)
			return -1;
		return (clientRequest->setServerResponse(readHtml("413", server, CODE413, "")), 0);
	}
	if ((size_t)contentLength> clientRequest->getBody().size())
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
	std::string port = clientMap[i]->getValueHeader("port"), method;
	servIt server = findIf(port, servers);
	if (checkServerName(clientMap[i], server) == -1)
		throw HttpException(CODE400, "bad server name");
	socketConfig.clients[i].events = POLLOUT;
	method = clientMap[i]->getValueHeader("method");
	if (method != "GET" && method != "POST" && method != "DELETE")
		return (clientMap[i]->setServerResponse(errorHtml(CODE405)), 0);
	if (port.empty())
		return (clientMap[i]->setServerResponse(errorHtml(CODE414)), 0);
	if (server == servers.end())
		return -1;
	int isTooLarge = checkLenBody(clientMap[i], server, size);
	if ( isTooLarge < 1)
		return isTooLarge;
	std::string clientUrl = clientMap[i]->getValueHeader("url"), file;
	locIt location = whichLocation(server, clientMap[i], clientUrl, "url");
	if (location != server->locations.end() && !location->redirCode.empty())
		return (clientMap[i]->setServerResponse(redir(server, location)), 0);
	file = createUrl(server, clientMap[i], location);

	std::cout << CYAN << method << RESET << " " << file << " " << clientMap[i]->getValueHeader("protocol") << std::endl;

	if (isCGIFile(clientUrl) && !isCGIAllowed(clientUrl, server, clientMap[i]))
		return (clientMap[i]->setServerResponse(readHtml("403", server, CODE403, "")), 0);
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
		return clientRequest->setServerResponse(readHtml("405", server, CODE405, ""));
	if (!isCGIFile(clientUrl))
		return clientRequest->setServerResponse(readHtml(file, server, CODE200, clientUrl));
	std::string root = (location != server->locations.end() && !location->root.empty()) ? location->root : server->root;
	std::string uploadLoc = uploadLocation(server, clientRequest);
	std::string output = executeCGI(clientUrl, root, clientRequest->getHeaderMap(), clientRequest->getBody(), uploadLoc);
	return clientRequest->setServerResponse(output);
}

void handlePostMethod(servIt server, locIt location, ClientRequest *clientRequest, std::string clientUrl, std::string file) {
	if (!isMethodAllowed("POST", server, clientRequest, ""))
		return clientRequest->setServerResponse(readHtml("405", server, CODE405, ""));
	if (!isCGIFile(clientUrl))
		return clientRequest->setServerResponse(readHtml(file, server, CODE200, clientUrl));
	std::string root = (location != server->locations.end() && !location->root.empty()) ? location->root : server->root;
	std::string uploadLoc = uploadLocation(server, clientRequest);
	std::string output = executeCGI(clientUrl, root, clientRequest->getHeaderMap(), clientRequest->getBody(), uploadLoc);
	return clientRequest->setServerResponse(output);
}

void handleDeleteMethod(servIt server, ClientRequest *clientRequest, std::string file) {
	if (!isMethodAllowed("DELETE", server, clientRequest, ""))
		return clientRequest->setServerResponse(readHtml("405", server, CODE405, ""));
	return clientRequest->setServerResponse(httpResponse("", "", handleDeleteMethod(server, file)));
}

void initSocket(t_socket &socketConfig, std::vector<t_server> &servers) {
	int	i = 0;
	struct addrinfo hints, *res;
	for (servIt it = servers.begin(); it != servers.end(); ++it) {
		try {
			initAddrInfo(servers, i, &hints, &res);

			socketConfig.serverFd.push_back(socket(res->ai_family, res->ai_socktype, 0));

			if (socketConfig.serverFd[i] < 0) {
				freeaddrinfo(res);
				throw std::runtime_error("socket fail");
			}
			int opt = 1;
			if (setsockopt(socketConfig.serverFd[i], SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
				freeaddrinfo(res);
				throw std::runtime_error("setsockopt fail");
			}
			if (bind(socketConfig.serverFd[i], res->ai_addr, res->ai_addrlen) < 0) {
				freeaddrinfo(res);
				throw std::runtime_error("bind fail");
			}
			freeaddrinfo(res);
			if (listen(socketConfig.serverFd[i], 5) < 0)
				throw std::runtime_error("listen fail");
			socketConfig.clients[i].fd = socketConfig.serverFd[i];
			socketConfig.clients[i].events = POLLIN | POLLOUT;
			std::cout << "[-] Running server " << CYAN << it->name << RESET << " on port " << CYAN << it->port << RESET << std::endl;
		}
		catch (const std::exception& e) {
			socketConfig.clients[i].fd = 0;
			std::cerr << e.what() << std::endl;
		}
		++i;
	}
}

void handleSocket(std::vector<t_server> &servers, t_socket &socketConfig) {
	cMap clientMap;
	socketConfig.clients = new struct pollfd[MAX_CLIENTS + servers.size()];
	socketConfig.clientCount = servers.size();
	std::memset(socketConfig.clients, 0, (MAX_CLIENTS + servers.size()) * sizeof(struct pollfd));
	initSocket(socketConfig, servers);
	setupSignalHandler();
	while (!stopRequested) {
		if (poll(socketConfig.clients, socketConfig.clientCount, 0) < 0)
			continue;
		for (int i = 0; i < socketConfig.clientCount; ++i) {
			try {
				if (socketConfig.clients[i].revents & POLLIN && socketConfig.clients[i].fd != 0) {
					if (handlePollin(socketConfig, servers, clientMap, i) == -1) {
						socketConfig.clients[i].events = POLLIN;
						continue;
					}
					if (i >= (int)servers.size()) {
						clientMap[i]->clearHeader();
						clientMap[i]->clearBody();
					}
				}
			} catch (const HttpException& e) {
				socketConfig.clients[i].events = POLLOUT;
				clientMap[i]->setServerResponse(errorHtml(e.getCodeMsg()));
			} catch (const HttpServerException& e) {
				socketConfig.clients[i].events = POLLOUT;
				clientMap[i]->setServerResponse(errorPage(e.getServ() ,e.getCodeMsg()));
			} catch (const std::exception& e) {
				std::cerr << RED << e.what() << RESET << std::endl;
				continue;
			}
			if (socketConfig.clients[i].revents & POLLOUT && socketConfig.clients[i].fd != 0) {
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
		if (socketConfig.clients[i].fd > 0)
			delete clientMap[i];
		close(socketConfig.clients[i].fd);
	}
	delete[] socketConfig.clients;
}
